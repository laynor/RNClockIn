let tryApply = (fn, ~default=None, value) =>
  switch (value, default) {
  | (Some(v), _) => Some(fn(v))
  | (None, d)    => d
  };
