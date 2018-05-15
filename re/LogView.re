open BsReactNative;
open MomentRe;
open Db.Stats;

type item = FlatList.renderBag((string, daystats));

let header_height = 64.;
let footer_height = 64.;

let styles =   StyleSheet.create(Style.({
  "card":
    style([padding(Pt(16.)),
           paddingTop(Pt(8.)),
           paddingBottom(Pt(8.)),
           elevation(0.7),
           shadowColor(String("black")),
           shadowOffset(~height=30., ~width=4.),
           shadowRadius(4.),
           borderRadius(6.),
           ]),

  "date":
    style([fontSize(Float(24.)),
           flex(1.),
           fontWeight(`Bold),
           textAlign(Center),
           textAlignVertical(Center)]),

  "entry":
    style([fontSize(Float(13.)),
           textAlignVertical(Center),
           margin(Pt(4.))]),

  "stats":
    style([fontSize(Float(16.))]),

  "cardItem":
    style([flexDirection(Row), flex(1.)]),
}));


let cardStyle = (style) => Style.concat([styles##card, style]);


module Day = {
  let component = ReasonReact.statelessComponent("Day");

  let make = (~date:string, ~total:Duration.t, ~balance:Duration.t, children) => {
    ...component,
    render: _self => {
      let cardColor = Duration.asHours(balance) |> Util.dayBalanceColor;
      let total = total |> Util.MomentExt.formatDuration;
      let balance = balance |> Util.MomentExt.formatDuration;

      <View style=cardStyle(Style.(style([margin(Pt(4.))])))>
        <View style=styles##cardItem>
          <Text value=date style=styles##date />
          <View style=cardStyle(Style.(style([backgroundColor(String(cardColor)),])))>
            <View style=styles##cardItem>
              <Text style=styles##stats value={j|Σ = $total\nΔ = $balance|j} />
            </View>
          </View>
        </View>

        <View style=Style.(style([marginTop(Pt(20.))]))>
          ...children
        </View>
      </View>
    }
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
            <View style=Style.(style([flex(1.), flexDirection(Column), height(Pt(70.)), alignContent(Center)])) >
              <Text style=styles##entry value=value />
              <View style=Style.(style([borderBottomColor(String("#CCCCCC")), borderBottomWidth(1.)])) />
            </View>
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
    <View style=Style.(style([margin(Pt(4.))]))>
      <FlatList data=stats renderItem=FlatList.renderItem(renderItem) keyExtractor=keyExtractor />
    </View>
  }
};
