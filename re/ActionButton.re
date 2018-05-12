[@bs.module "react-native-action-button"] external actionButton:ReasonReact.reactClass = "default";

let make = (~style=?, ~renderIcon=?, ~onPress=?, ~buttonColor=?, children) =>
  ReasonReact.wrapJsForReason(
  ~reactClass=actionButton,
  ~props={
    "renderIcon":  Js.Undefined.fromOption(renderIcon),
    "style":       Js.Undefined.fromOption(style),
    "onPress":     Js.Undefined.fromOption(onPress),
    "buttonColor": Js.Undefined.fromOption(buttonColor)
  },
  children
);
