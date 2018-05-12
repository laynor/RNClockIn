open BsReactNative;
open MomentRe;

module MyApp = {
  type state = {db:Db.db};

  type action = Click | Rehydrate(Db.db);

  let component = ReasonReact.reducerComponent("MyApp");

  let persist(db:Db.db) = {
    /* convert state to JSON */
    let stateAsJson = Db.toJson(db);
    Js.Console.warn(stateAsJson);
    let _ = AsyncStorage.setItem(
      "ClockIn.state",
      stateAsJson,
      ~callback=
        (e) =>
        switch e {
        | None => ()
        | Some(err) => Js.log(err)
        },
      ()
    );
    ()
  };

  let rehydrate = (self:ReasonReact.self(state, ReasonReact.noRetainedProps, action)) => {
    Js.Promise.(
      /* begin call to AsyncStorage */
      AsyncStorage.getItem("ClockIn.state", ())
      |> then_(
          json =>
            switch json {
            | None => ()
            | Some(s) =>
              /* parse JSON, decode it into a ReasonML Record, and reset the state */
              let db = Db.fromJson(s);
              self.send(Rehydrate(db));
              ()
            }|> resolve) |> ignore
    );
  };

  let onClickAction = (s:state) => {
    let db = s.db;
    let action = Db.punchedin(db) ? Db.Action.punchout : Db.Action.punchin;
    let newdb = action(momentNow(), db);
    ReasonReact.UpdateWithSideEffects({db:newdb},
                                      _self => persist(newdb))
  };

  let styles = StyleSheet.create(Style.({
    "container":
      style([flex(1.),
             justifyContent(Center),
             alignItems(Center)]),

    "logViewWrapper":
      style([flex(1.),
             backgroundColor(Colors.light),
             width(Pct(100.)),
             padding(Pt(20.))]),

    "buttonWrapper":
      style([flex(0.22),
             margin(Pt(10.))])
  }));

  let renderRecordIcon = (_) => <RNIcons.Entypo name ="controller-record" color =Colors.black />;
  let renderStopIcon   = (_) => <RNIcons.Entypo name ="controller-stop" color   =Colors.red />;

  let make = (_children) => {
    ...component,

    initialState: () => {db:[]},

    reducer: (a: action, s:state) =>
      switch(a) {
      | Click => onClickAction(s)
      | Rehydrate(db) => ReasonReact.Update({db:db})
      },

    didMount: self =>
      rehydrate(self),

    render: self => {
      let onClick = () => self.send(Click);

      let stats   = self.state.db |> Db.Stats.dbstats;

      let day2Section = (day:Db.day) => {
        SectionList.section(~data=Array.of_list(day.entries), ~key=day.date, ());
      };

      let sections = self.state.db |> List.map(day2Section) |> Array.of_list |> SectionList.sections;

      let (renderIcon, color) = Db.punchedin(self.state.db) ? (renderStopIcon, Colors.black) : (renderRecordIcon, Colors.red);

      <View style=styles##container>
        <View style=styles##logViewWrapper>
          <LogView sections=sections stats=stats />
        </View>
        <Summary stats=stats size=50. />
        <ActionButton renderIcon=renderIcon buttonColor=color onPress=onClick />
      </View>;
    }
  }
};

let app = () => {
    <MyApp />
};
