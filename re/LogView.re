open BsReactNative;
open MomentRe;
open Db.Stats;

type entryData   = {punchedout: bool, elapsed: Duration.t, entry: Db.entry};
type sectionData = {date: string, total: float, balance: float};
type dayData     = {date: string, total: float, balance: float, entries: array(entryData)};
type item        = FlatList.renderBag(Db.day);

let entryData = (e:Db.entry):entryData =>
  switch(e) {
  | {start:s, end_:None} => {punchedout: false,
                             elapsed: durationMillis(truncate(0.5 +. diff(momentNow(), s, `milliseconds))),
                             entry: {start:s,
                                     end_: None}};

  | {start:s, end_:Some(e)} => {punchedout:true,
                                elapsed: durationMillis(truncate(0.5 +. diff(e, s, `milliseconds))),
                                entry: {start: s,
                                        end_: Some(e)}}
  };

module Day = {
  let component = ReasonReact.statelessComponent("Day");
  let make = (~date, ~total, ~balance, children) => {
    ...component,
    render: _self => {
      let date' = date|> Moment.format(Prefs.date_format);

      NativeBase.(
        <Card>
          <CardItem  header=true>
            <NativeBase.Text value=date'/>
          </CardItem>
          <CardItem>
            <Summary.StatView title="Tot" size=30. value={j|$total|j} />
            <Summary.StatView title="Bal" size=30. value={j|$balance|j} />
          </CardItem>
          <View>
              ...children
          </View>
        </Card>
      )
    }
  }
};

module Entry = {
  let component = ReasonReact.statelessComponent("Entry");
  let make = (~punchinTime, _children) => {
    ...component,
    render: _self => {
      NativeBase.(
        <CardItem header=(false)>
          <NativeBase.Text value=punchinTime />
        </CardItem>
      )
    }
  }
};

let findDayStats = (date, stats) =>
  switch (find(date, stats)) {
  | Some(stats') => stats'
  | None         => emptyDay
  };

let renderDay = (stats:dbstats, day:Db.day) => {
  let date = day.date;
  let stats = findDayStats(date, stats);
  let children =
    day.entries
    |> List.map((entry:Db.entry) => Moment.format("HH:MM", entry.start))
    |> List.map((punchinTime) => <Entry punchinTime=punchinTime />)
    |> Array.of_list;
  <Day date=moment(day.date) total=stats.total balance=stats.balance>
    ...children
  </Day>
};

let component = ReasonReact.statelessComponent("LogView");

let keyExtractor = (item, i) => {j|$i$item|j};

let renderItem = (stats, item: item) => renderDay(stats, item.item);

let make = (~stats, ~days:array(Db.day), _children) => {
  ...component,
  render: _self => {
    /* FIXME should hash this one */
    <FlatList data=days renderItem=FlatList.renderItem(renderItem(stats)) keyExtractor=keyExtractor />
  }
};
