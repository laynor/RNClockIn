[@bs.module "native-base"] external container: ReasonReact.reactClass = "Container";
[@bs.module "native-base"] external header: ReasonReact.reactClass = "Header";
[@bs.module "native-base"] external content: ReasonReact.reactClass = "Content";
[@bs.module "native-base"] external card: ReasonReact.reactClass = "Card";
[@bs.module "native-base"] external cardItem: ReasonReact.reactClass = "CardItem";
[@bs.module "native-base"] external body: ReasonReact.reactClass = "Body";
[@bs.module "native-base"] external text: ReasonReact.reactClass = "Text";

module Card = {
  let make = (children) => {
    ReasonReact.wrapJsForReason(~reactClass=card,
                                ~props=Js.Null.empty,
                                children);
  }
};

module CardItem = {
  let make = (~header=?, children) => {
    ReasonReact.wrapJsForReason(~reactClass=cardItem,
                                ~props={"header" : Js.Undefined.fromOption(header)},
                                children);
  }
};

module Text = {
  let make = (~value=?, _children) => {
    ReasonReact.wrapJsForReason(~reactClass=text,
                                ~props=Js.Null.empty,
                                value);
  }
};
