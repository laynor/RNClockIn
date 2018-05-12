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

  type entrystats = {entry: entry, duration: Duration.t};

  type daystats = {date: string, total: Duration.t, balance: Duration.t, entries: array(entrystats)};

  type dbstats  = {
    days: StringMap.t(daystats),
    n_days: int,
    total: Duration.t,
    expected: Duration.t,
    balance: Duration.t,
    average: Duration.t,
  };

  let entryDuration = (e:entry): Duration.t =>
    switch e.end_ {
    | None => diff(momentNow(), e.start, `milliseconds)
    | Some(m) => diff(m, e.start, `milliseconds);
    } |> truncate |> durationMillis;


  let test = () => {
    Js.Console.warn(diff(moment("2018-05-05 15:30"), moment("2018-05-05 15:50"), `hours));
  };

  let dayTotal = (day:day) => {
    let minutes =
      day.entries
      |> List.map(entryDuration)
      |> List.map(Duration.asMinutes)
      |> List.fold_left((x, y) => x +. y, 0.);

    duration(truncate(minutes), `minutes)
  };

  let daystats = (day:day) => {
    let entries =
      day.entries
      |> List.map(entry => {entry: entry, duration: entryDuration(entry)})
      |> Array.of_list;
    let total = dayTotal(day);
    {
      total: total,
      balance: Util.MomentExt.dursub(total, durationFormat(Prefs.worktime)),
      date: day.date,
      entries: entries
    }
  };


  let dbstats = (db:db):dbstats => {
    open Util.MomentExt;
    let empty = StringMap.empty;
    let days  = List.fold_left((stats, day:day) => StringMap.add(day.date, daystats(day), stats), empty, db);
    let n_days = StringMap.cardinal(days);
    let expected = durmul(float(n_days), durationFormat(Prefs.worktime));
    let total = StringMap.fold((_, value:daystats, acc) => dursum([acc, value.total]),
                               days,
                               durzero);
    let balance = dursub(total, expected);
    let average = durmul(1.0 /. float(n_days), total);
    {days, total, n_days, expected, balance, average}
  };


  let emptyDay(date) = Util.MomentExt.{total: durzero, balance: durzero, date: date, entries: [||]};
  let defaultStat = entry => emptyDay(Moment.format(Prefs.date_format, entry.start));

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
