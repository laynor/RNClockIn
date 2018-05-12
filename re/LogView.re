open BsReactNative;
open MomentRe;
open Db.Stats;

type entryData   = {punchedout: bool, elapsed: Duration.t, entry: Db.entry};
type sectionData = {date: string, total: float, balance: float};

type item      = SectionList.renderBag(Db.entry);
type accessory = SectionList.renderAccessory(Db.entry);

let entryData = (e:Db.entry):entryData => {
  switch(e) {
  | {start:s, end_:None} => {punchedout: false,
                             elapsed: durationMillis(truncate(0.5 +. diff(momentNow(), s, `milliseconds))),
                             entry: {start:s,
                                     end_: None}};
  | {start:s, end_:Some(e)} => {punchedout:true,
                                elapsed: durationMillis(truncate(0.5 +. diff(e, s, `milliseconds))),
                                entry: {start: s,
                                        end_: Some(e)}}
  };
};

let listStyle = Style.(style([
  backgroundColor("#ddddee"),
  width(Pct(100.))
]));

module SectionHeader = {
  let component = ReasonReact.statelessComponent("SectionHeader");
  let make = (~date, ~total, ~balance, _children) => {
    ...component,
    render: _self => {
      let date' = date |> moment |> Moment.format("LT");
      <View>
        <Text value = {j|$date' total: $total balance: $balance |j} style = Style.(style([backgroundColor(Colors.log_header_color)])) />
      </View>
    }
  }
};

module ListItem = {
  let component = ReasonReact.statelessComponent("ListItem");
  let make = (~entryData as data:entryData, _children) => {
    ...component,
    render: _self => {
      let {elapsed: elap, entry: {start: s, end_: e}} = data;

      let start = Moment.format("YYYY-MM-DD HH:mm:ss", s);

      let dur   = Duration.humanize(elap);

      let v =
        switch(e) {
        | Some(e2) =>
          let end_ = Moment.format("YYYY-MM-DD HH:mm:ss", e2);
          {j|out= $end_ ($dur) |j}

        | None => {j|in @$start|j}
        };

      <TouchableOpacity style=Style.(style([height(Pt(30.))]))>
        <Text value=v />
      </TouchableOpacity>
    }
  }
};

let component = ReasonReact.statelessComponent("LogView");

let findDayStats = (date, stats) =>
  switch date {
  | Some(date') =>
    switch (find(date', stats)) {
    | Some(stats') => stats'
    | None => emptyDay
    };
  | None => emptyDay
  };

let keyExtractor = (item, i) => {j|$i$item|j};

let renderItem = (item:item) => <ListItem entryData=entryData(item.item) />;

let renderSectionHeader = (stats, foo:accessory) => {
  let date = foo.section.key;
  let {total: total, balance: balance}:daystats = findDayStats(date, stats);
  let d = switch date {
  | None => "<missing>"
  | Some(d') => d'
  };
  <SectionHeader date=d total=total balance=balance />
};


let make = (~stats, ~sections, _children) => {
  ...component,

  render: _self => {
    /* FIXME should hash this one */
    <SectionList sections            = sections
                 renderItem          = SectionList.renderItem(renderItem)
                 renderSectionHeader = SectionList.renderAccessoryView(renderSectionHeader(stats))
                 keyExtractor        = keyExtractor />;
  }
};
