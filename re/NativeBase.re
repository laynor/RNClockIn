[@bs.module "native-base"] external container: ReasonReact.reactClass = "Container";
[@bs.module "native-base"] external header: ReasonReact.reactClass = "Header";
[@bs.module "native-base"] external footer: ReasonReact.reactClass = "Footer";
[@bs.module "native-base"] external left: ReasonReact.reactClass = "Left";
[@bs.module "native-base"] external right: ReasonReact.reactClass = "Right";
[@bs.module "native-base"] external title: ReasonReact.reactClass = "Title";
[@bs.module "native-base"] external content: ReasonReact.reactClass = "Content";
[@bs.module "native-base"] external card: ReasonReact.reactClass = "Card";
[@bs.module "native-base"] external cardItem: ReasonReact.reactClass = "CardItem";
[@bs.module "native-base"] external body: ReasonReact.reactClass = "Body";
[@bs.module "native-base"] external text: ReasonReact.reactClass = "Text";
[@bs.module "native-base"] external icon: ReasonReact.reactClass = "Icon";
[@bs.module "native-base"] external button: ReasonReact.reactClass = "Button";
[@bs.module "native-base"] external fab: ReasonReact.reactClass = "Fab";
[@bs.module "native-base"] external view: ReasonReact.reactClass = "View";

module Card = {
  let make = (children) => {
    ReasonReact.wrapJsForReason(~reactClass=card,
                                ~props=Js.Obj.empty(),
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
                                ~props=Js.Obj.empty(),
                                value);
  }
};


module Container = {
  let make = (children) => {
    ReasonReact.wrapJsForReason(~reactClass=container,
                                ~props=Js.Obj.empty(),
                                children);
  }
};

module Header = {
  let make = (children) => {
    ReasonReact.wrapJsForReason(~reactClass=header,
                                ~props=Js.Obj.empty(),
                                children);
  }
};


module Footer = {
  let make = (~style=?, children) => {
    ReasonReact.wrapJsForReason(~reactClass=footer,
                                ~props={"style": Js.Undefined.fromOption(style)},
                                children);
  }
};

module View = {
  let make = (~style=?, children) => {
    ReasonReact.wrapJsForReason(~reactClass=view,
                                ~props={"style": Js.Undefined.fromOption(style)},
                                children);
  }
};

module Left = {
  let make = (children) => {
    ReasonReact.wrapJsForReason(~reactClass=left,
                                ~props=Js.Obj.empty(),
                                children);
  }
};

module Body = {
  let make = (children) => {
    ReasonReact.wrapJsForReason(~reactClass=body,
                                ~props=Js.Obj.empty(),
                                children);
  }
};

module Right = {
  let make = (children) => {
    ReasonReact.wrapJsForReason(~reactClass=right,
                                ~props=Js.Obj.empty(),
                                children);
  }
};

module Title = {
  let make = (~value=?, _children) => {
    ReasonReact.wrapJsForReason(~reactClass=title,
                                ~props=Js.Obj.empty(),
                                value);
  }
};

module Icon = {
  let make = (~name, children) => {
    ReasonReact.wrapJsForReason(~reactClass=icon,
                                ~props={"name": name},
                                children);
  }
};

module Button = {
  let make = (~style=?, ~transparent=?, children) => {
    ReasonReact.wrapJsForReason(~reactClass=button,
                                ~props={"transparent": Js.Undefined.fromOption(transparent),
                                        "style": Js.Undefined.fromOption(style)},
                                children);
  }
};

module Content = {
  let make = (children) => {
    ReasonReact.wrapJsForReason(~reactClass=content,
                                ~props=Js.Obj.empty(),
                                children);
  }
};


module Fab = {
  let make = (~onPress=?, children) => {
    ReasonReact.wrapJsForReason(~reactClass=fab,
                                ~props={"onPress": Js.Undefined.fromOption(onPress),
                                        "containerStyle": Js.Obj.empty(),
                                        "position": "bottomRight",
                                        "style": Js.Obj.empty(),
                                        "active": false,
                                        "children": children},
                                children);
  }
};
