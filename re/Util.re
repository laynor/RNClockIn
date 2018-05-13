
let tryApply = (fn, ~default=None, value) =>
  switch (value, default) {
  | (Some(v), _) => Some(fn(v))
  | (None, d)    => d
  };

let sign = (n:int):int =>
  switch n {
  | 0 => 0
  | _ => n / abs(n)
  };

let sign_float = (x:float):int =>
  switch x {
  | 0.0 => 0
  | _ => truncate(x /. abs_float(x))
  };

module MomentExt = {
  open MomentRe;
  type dt = Duration.t;

  let durzero:dt = durationMillis(0);

  let dursum = (durations:list(dt)): dt => {
    durations
    |> List.map(Duration.asMilliseconds)
    |> List.fold_left((x, y) => x +. y, 0.)
    |> truncate
    |> durationMillis
  };

  let dursub = (dur1:dt, dur2:dt) => {
    Duration.asMilliseconds(dur1) -. Duration.asMilliseconds(dur2)
    |> truncate
    |> durationMillis
  };

  let durmul = (n:float, d:dt) =>
    n *.  Duration.asMilliseconds(d) |> truncate |> durationMillis;


  let negateDuration = (d:dt) => {
    let millis = d |> Duration.asMilliseconds;
    durationMillis(-truncate(millis));
  };

  let sign_duration = (d:dt) => Duration.asMilliseconds(d) |> sign_float;

  let formatDuration = (d:dt) => {
    let hours = d |> Duration.asHours |> truncate;
    let minutes = d |> Duration.minutes |> abs;
    {j|$hours:$minutes|j}
  };

};
