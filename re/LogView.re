open BsReactNative;
open MomentRe;
open Db.Stats;

type item = FlatList.renderBag((string, daystats));

let header_height = 64.;
let footer_height = 64.;

let styles =   StyleSheet.create(Style.({
  "card":
    style([margin(Pt(4.)),
           padding(Pt(16.)),
           paddingTop(Pt(8.)),
           paddingBottom(Pt(8.)),
           elevation(2.),
           shadowColor(String("black")),
           shadowOffset(~height=30., ~width=4.),
           shadowRadius(4.),
           borderRadius(6.),
           borderColor(String("#AAAAAA"))]),

  "date":
    style([fontSize(Float(24.)),
           textAlignVertical(Center)]),

  "entry":
    style([fontSize(Float(13.)),
           margin(Pt(4.))]),

  "stats":
    style([fontSize(Float(16.))]),

  "cardItem":
    style([flexDirection(Row), flex(1.)]),
}));

module Day = {
  let component = ReasonReact.statelessComponent("Day");

  let make = (~date:string, ~total:string, ~balance:string, children) => {
    ...component,
    render: _self =>
      <View style=styles##card>
        <View style=styles##cardItem>
          <Text value=date style=styles##date />
          <View style=Style.(style([flex(1.)])) />
          <View style=styles##card>
            <View style=styles##cardItem>
              <Text style=styles##stats value={j|total: $total\nbalance: $balance|j} />
            </View>
          </View>
        </View>

        <View>
          ...children
        </View>
      </View>
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
        <View style=styles##cardItem>
          <Text style=styles##entry value=value />
        </View>
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
