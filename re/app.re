open BsReactNative;

module Cane = {
  let foo = (x, y) => x + y;
  let bar = (x, y, z) => x ++ y ++ z;
};

module MyButton = {
  let component = ReasonReact.statelessComponent("MyButton");
  let make = (_children) => {
    <Text value="to do" />
  };
};

module MyApp = {
  type state = {foo: string};
  type action = Click;
  let component = ReasonReact.reducerComponent("MyApp");
  let make = (_children) => {
    ...component,
    initialState: () => {foo: "antnai"},
    reducer: (a: action, s:state) =>
      switch(a) {
      | Click => ReasonReact.Update(s)
      },
    render: self => {
      <View style=Style.(style([flex(1.), justifyContent(Center), alignItems(Center)]))>
        <Text value=self.state.foo />
        <Text value="Reason is awesome!" />
      </View>;
    }
  }
};

let app = () => {
  <View style=Style.(style([flex(1.), justifyContent(Center), alignItems(Center)]))>
    <Text value="Reason is awesome!" />
  </View>;
}
