open BsReactNative;
open MomentRe;

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

  let renderRecordIcon = (_) => <RNIcons.Entypo name ="controller-record" color =Colors.black />;
  let renderStopIcon   = (_) => <RNIcons.Entypo name ="controller-stop" color   =Colors.red />;

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

      let (renderIcon, buttonColor) =
        Db.punchedin(self.state.db)
        ? (renderStopIcon, Colors.black)
        : (renderRecordIcon, Colors.red);

      let stats = self.state.db |> Db.Stats.dbstats;
      let statsArray = stats |> Db.Stats.daysStatsAsArray;

      /* let fabContainerStyle = Style.(style([ */
      /*   height(Pt(100.)), */
      /*   width(Pt(100.)), */
      /*   zIndex(100000000000), */
      /*   right(Pt(16.)), */
      /*   bottom(Pt(16.)), */
      /*   position(Absolute)])); */

      NativeBase.(
        <Container>
          <Header>
            <Left>
              <NativeBase.Button transparent=true>
                <Icon name="menu" />
              </NativeBase.Button>
            </Left>
            <Body>
              <Title value="antani" />
            </Body>
          </Header>
          <Content>
            <NativeBase.View>
              <LogView stats=statsArray />
            </NativeBase.View>
          </Content>

          /* <NativeBase.View style=fabContainerStyle> */
          /*   <ActionButton renderIcon buttonColor onPress /> */
          /* </NativeBase.View> */

          <Fab onPress>
            <NativeBase.Icon name="mail" />
            /* <NativeBase.Button style=Style.(style([backgroundColor(buttonColor)]))> */
            /*   <NativeBase.Text value="foo" /> */
            /* </NativeBase.Button> */
            /* <NativeBase.Button style=Style.(style([backgroundColor(buttonColor)]))> */
            /*   <NativeBase.Text value="foo" /> */
            /* </NativeBase.Button> */
          </Fab>
          <Footer>
            <Summary stats=stats size=50. />
          </Footer>
        </Container>
      )
    }
  }
};

let app = () => {
    <MyApp />
};
