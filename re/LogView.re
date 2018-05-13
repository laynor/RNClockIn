open BsReactNative;
open MomentRe;
open Db.Stats;

type item = FlatList.renderBag((string, daystats));

module Day = {
  let component = ReasonReact.statelessComponent("Day");

  let make = (~date:string, ~total:string, ~balance:string, children) => {
    ...component,
    render: _self =>
      NativeBase.(
        <Card>
          <CardItem header=true >
            <NativeBase.Text value=date />
            <View style=Style.(style([flex(1.)])) />
            <Card>
              <CardItem>
                <NativeBase.Text value={j|total: $total\nbalance: $balance|j} />
              </CardItem>
            </Card>
          </CardItem>

          <View>
              ...children
          </View>
        </Card>
      )
  }
};

module Entry = {
  let component = ReasonReact.statelessComponent("Entry");

  let make = (~punchinTime, ~punchoutTime=?, ~duration,  _children) => {
    ...component,
    render: _self => {
      let value = switch punchoutTime {
        | Some(_) => {j|$(punchinTime)~$(punchoutTime) ($(duration))|j}
        | None    => {j|$punchinTime ($(duration))|j}
        };
      NativeBase.(
        <CardItem header=(false)>
          <NativeBase.Text value=value />
        </CardItem>
      )
    }
  }
};

let renderDay = ((_, {date, total, balance, entries})) => {
  let fmtTime = Moment.format("HH:mm");

  let children =
    entries
    |> Array.map((s:entrystats) => {
        let entry = s.entry;
        let start = fmtTime(entry.start);
        let end_  = Util.tryApply(fmtTime, entry.end_);
        let dur   = s.duration |> Duration.humanize;

        <Entry punchinTime=start punchoutTime=?end_ duration=dur />
      });
  let total = total |> Util.MomentExt.formatDuration;
  let balance = balance |> Util.MomentExt.formatDuration;
  let date = moment(date) |> Moment.format(Prefs.date_format);
  <Day date total balance>
    ...children
  </Day>
};

let component = ReasonReact.statelessComponent("LogView");

/* FIXME should hash this one */
let keyExtractor = (item, i) => {j|$i$item|j};

let renderItem = (item: item) => renderDay(item.item);

let make = (~stats:array((string, daystats)), _children) => {
  ...component,
  render: _self => {
    <FlatList data=stats renderItem=FlatList.renderItem(renderItem) keyExtractor=keyExtractor />
  }
};
