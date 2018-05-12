open BsReactNative;
open Db.Stats;

module StatView = {
  let component = ReasonReact.statelessComponent("StatView");
  let containerStyle = (size, color') =>
    Style.(style([height(Pt(size)),
                  width(Pt(size)),
                  flexDirection(Column),
                  justifyContent(Center),
                  alignItems(Center),
                  borderRadius(size /. 10.0),
                  margin(Pt(4.)),
                  backgroundColor(color')]));
  let textStyle = (color', size) =>
    Style.(style([alignSelf(Center),
                  color(color'),
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
    style([flexDirection(Row),
           justifyContent(Center),
           alignItems(Center)]),
}));

let make = (~stats as {average, balance}:dbstats, ~size=?, _children) => {
  ...component,
  render: _self => {
    <View style=styles##stats>
        <StatView title="Avg" size=?size value={j|$average|j} />
        <StatView title="Bal" size=?size value={j|$balance|j} />
    </View>
  }
};
