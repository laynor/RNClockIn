open BsReactNative;
open MomentRe;

let header_height = 64.;
let footer_height = 64.;
let fabRadius = 200.;

let styles =   StyleSheet.create(Style.({
  "container":
    style([flex(1.)]),

  "header":
    style([width(Pct(100.)),
           height(Pt(header_height)),
           flexDirection(Row),
           backgroundColor(String(Colors.blue_primary))]),

  "footer":
    style([width(Pct(100.)),
           height(Pt(footer_height)),
           bottom(Pt(0.)),
           flexDirection(Row),
           backgroundColor(String(Colors.blue_primary))]),

  "fabStyle":
    style([height(Pct(100.)),
           elevation(2.),
           width(Pct(100.)),
           zIndex(100)]),

  "left":
    style([flex(1.), borderWidth(1.), borderColor(String(Colors.black))]),

  "body":
    style([flex(1.)]),

  "content":
    style([flex(1.)]),

  "right":
    style([flex(1.)])
}));

let module Container = {
  let component = ReasonReact.statelessComponent("Container");

  let make = (children) => {
    ...component,
    render: _self =>
      <View style=styles##container>
        ...children
      </View>
  }
};

module MyApp = {
  type state = {db:Db.db, fabActive:bool};

  type action = Click | Rehydrate(Db.db);

  let component = ReasonReact.reducerComponent("MyApp");

  let persist(db:Db.db) = {
    /* convert state to JSON */
    let stateAsJson = Db.toJson(db);
    AsyncStorage.setItem(
      "ClockIn.state",
      stateAsJson,
      ~callback=
        (e) =>
        switch e {
        | None => ()
        | Some(err) => Js.log(err)
        },
      ()) |> ignore;
  };

  let rehydrate = (self:ReasonReact.self(state, ReasonReact.noRetainedProps, action)) => {
    Js.Promise.(
      /* begin call to AsyncStorage */
      AsyncStorage.getItem("ClockIn.state", ())
      |> then_(
          json => switch json {
          | None => ()
          | Some(s) =>
            /* parse JSON, decode it into a ReasonML Record, and reset the state */
            let db = Db.fromJson(s);
            self.send(Rehydrate(db));
          }|> resolve
        ) |> ignore
    );
  };

  let onClickAction = (s:state) => {
    let db = s.db;
    let action = Db.punchedin(db) ? Db.Action.punchout : Db.Action.punchin;
    let now = momentNow();
    let newdb = action(now, db);
    ReasonReact.UpdateWithSideEffects({...s, db:newdb},
                                      _self => persist(newdb))
  };

  let renderRecordIcon = (_) => <RNIcons.Entypo size=24. name="controller-record" color= Colors.black />;
  let renderStopIcon   = (_) => <RNIcons.Entypo size=24. name="controller-stop"   color= Colors.red />;

  let make = (_children) => {
    ...component,

    initialState: () => {db:[], fabActive: false},

    reducer: (a: action, s:state) =>
      switch(a) {
      | Click => onClickAction(s)
      | Rehydrate(db) => ReasonReact.Update({...s, db:db})
      },

    didMount: self =>
      rehydrate(self),

    render: self => {
      let onPress = () => { Js.Console.warn("pressed"); self.send(Click) };
      let onMenuPress = () => { Js.Console.warn("menu pressed") };

      let (renderIcon, buttonColor) =
        Db.punchedin(self.state.db)
        ? (renderStopIcon, Colors.black)
        : (renderRecordIcon, Colors.red);

      let stats = self.state.db |> Db.Stats.dbstats;
      let statsArray = stats |> Db.Stats.daysStatsAsArray;

      let module Body = View;
      let module Content = View;
      let module Fab = Button;
      let module Footer = View;

      <View style=styles##container>
        <View style=styles##header>
          <View style=styles##left>
          </View>
          <View style=styles##body>
            <Text value="antani" />
          </View>
          <View style=styles##right />
        </View>
        <View style=styles##content>
          <LogView stats=statsArray />
        </View>

        <View style=styles##footer>
          <Summary stats=stats size=50. />
        </View>
        <ActionButton renderIcon buttonColor onPress />
      </View>
    }
  }
};

let app = () => {
    <MyApp />
};
