open BsReactNative;
open Db.Stats;
open MomentRe;

module StatView = {
  let component = ReasonReact.statelessComponent("StatView");
  let containerStyle = (size, color') =>
    Style.(style([height(Pt(size)),
                  width(Pt(size)),
                  flexDirection(Column),
                  justifyContent(Center),
                  alignItems(Center),
                  elevation(1.),
                  borderRadius(size /. 10.0),
                  margin(Pt(4.)),
                  backgroundColor(String(color'))]));
  let textStyle = (color', size) =>
    Style.(style([alignSelf(Center),
                  color(String(color')),
                  fontSize(Float(size))]));

  let make = (~title, ~value, ~size=100.0, ~backgroundColor="green", ~color="white",  _children) => {
    ...component,
    render: _self => {
      <View style=containerStyle(size, backgroundColor)>
        <Text style=textStyle(color, size *. 0.30) value=title />
        <Text style=textStyle(color, size *. 0.25) value=value />
      </View>
    }
  };
};

let component = ReasonReact.statelessComponent("Summary");

let styles = StyleSheet.create(Style.({
  "stats":
    style([flexDirection(Column),
           justifyContent(Center),
           alignItems(Center)]),
}));

let make = (~stats as {average, balance}:dbstats, ~size=?, _children) => {
  ...component,
  render: _self => {
    open Util.MomentExt;

    let hours = balance |> Duration.asHours;

    let average = average |> formatDuration;
    let balance = balance |> formatDuration;

    let color = switch hours {
      | n when n >= 0.0 => Colors.green;
      | n when n <= Prefs.balance_warn_strong => Colors.red;
      | n when n <= Prefs.balance_warn_mild => Colors.yellow;
      | _ => Colors.light_green
    };

    <View style=styles##stats>
        <StatView title={j|μ|j} size=?size value={j|$average|j} />
        <StatView title={j|Δ|j} backgroundColor=color size=?size value={j|$balance|j} />
    </View>
  }
};
