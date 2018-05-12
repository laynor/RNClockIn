open MomentRe;

type db = list(day)
and day = {date: string, entries: list(entry)}
and entry = {start: Moment.t, end_: option(Moment.t) };

let dateFormat = "YYYY-MM-DD";
let isodate = Moment.format(dateFormat);


let mockDay = (date:string) : day => {
  let midnight = moment(date);

  let plus = (n, w) =>
    Moment.add(~duration=duration(n, w));

  let at = (~s=0, ~m=0, h) => {
    midnight
    |> plus(h, `hours)
    |> plus(m, `minutes)
    |> plus(s, `seconds)
  };

  {date: date, entries:[{start: at(9), end_:Some(at(14))}, {start: at(15, ~m=13), end_:Some(at(18))}]}
};


module Decode = {
  let entry = json : entry => Json.Decode.{
    start: json |> field("start", string) |> moment,
    end_: json |> optional(field("end_", string)) |> Util.tryApply(moment)
  };
  let day = json : day => Json.Decode.{
    date: json |> field("date", string),
    entries: json |> field("entries", list(entry))
  };
  let db = json : db => Json.Decode.list(day, json);
};


module Encode = {
  let encodeDate = d => Moment.toJSON(d);
  let encodeOptionalDateField = date =>
    switch(date) {
    | None => []
    | Some(date') => [("end_", Json.Encode.string(encodeDate(date')))]
    };
  let entry = (e:entry) => {
    let end_ = encodeOptionalDateField(e.end_);
    Json.Encode.(
    object_([
      ("start", Json.Encode.string(encodeDate(e.start))),
      ...end_
    ]));
  };

  let day = (d:day) => {
    Json.Encode.(
    object_([
      ("date", Json.Encode.string(d.date)),
      ("entries", Json.Encode.list(entry, d.entries))
    ]));
  };

  let db = (db:db) => Json.Encode.list(day, db);
};


let toJson = (db:db) =>
  Encode.db(db) |> Js.Json.stringify;

let fromJson = (json:string) =>
  json |> Js.Json.parseExn |> Decode.db;


let isEmpty = (a:array('a)) =>
  switch (Array.length(a)) {
  | 0 => true
  | _ => false
  };

let punchedin = (db:db):bool =>
  switch(db) {
  | [] => false
  | [day, ..._] =>
    switch day {
    | {date: _, entries: [{start: _, end_:None}, ..._]}  => true
    | _ => false
    };
  };

module Action = {
  let punchinDay = (day:day, time:Moment.t):day => {
    {...day, entries: [{start:time, end_:None}, ...day.entries]}
  };

  let punchin = (time:Moment.t, db:db):db => {
    let date = isodate(time);
    let newDay = {date: date, entries:[]};
    switch(db) {
    | [] => [punchinDay(newDay, time)]
    | [day, ...days] when day.date == date => [punchinDay(day, time), ...days]
    | _ => [punchinDay(newDay, time), ...db]
    }
  };

  let punchoutDay({date: date, entries: entries}, time) = {
    switch(entries) {
    | [{start: start, end_: None}, ...rest] =>
      {date: date, entries: [{start: start, end_: Some(time)}, ...rest]}
    | _ => failwith("cannot punchoutday")
    }
  };

  let punchout = (time:Moment.t, db:db) => {
    let date = isodate(time);
    switch (db) {
    | [day, ...days] when day.date == date => [punchoutDay(day, time), ...days]
    | _ => failwith("Cannot punchout")
    }
  };
};


module Stats = {
  module StringMap = Map.Make(String);

  type daystats = {total: float, balance: float};

  type dbstats  = {
    days: StringMap.t(daystats),
    n_days: int,
    total: float,
    expected: float,
    balance: float,
    average: float,
  };

  let entryHours = (e:entry) =>
    switch e.end_ {
    | None => 0.
    | Some(m) => diff(m, e.start, `hours);
    };

  let dayTotal = (day:day) => {
    day.entries
    |> List.map(entryHours)
    |> List.fold_left((x, y) => x +. y, 0.)
  };

  let dayStats = (day:day) => {
    let total = dayTotal(day);
    {total: total, balance: total -. Prefs.hours_a_day}
  };

  let dbstats = (db:db):dbstats => {
    let empty = StringMap.empty;
    let days  = List.fold_left((stats, day) => StringMap.add(day.date, dayStats(day), stats), empty, db);
    let n_days = StringMap.cardinal(days);
    let expected = float(n_days) *. Prefs.hours_a_day;
    let total = StringMap.fold((_, value:daystats, acc) => acc +. value.total, days, 0.0);
    let balance  = total -. expected;
    let average = total /. float(n_days);
    {days, total, n_days, expected, balance, average}
  };


  let emptyDay = {total: 0.0, balance: 0.0 };
  let defaultStat = _entry => emptyDay;

  let find = (date, db:dbstats) =>
    switch (StringMap.find(date, db.days)) {
    | stat => Some(stat)
    | exception Not_found => None
    };

  let daysStatsAsArray = (stats:dbstats) => StringMap.bindings(stats.days) |> Array.of_list;

  let days = stats => {
    stats.days
    |> StringMap.bindings
    |> List.map(((k, _v)) => k)
  };
};
