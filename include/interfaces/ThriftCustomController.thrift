namespace cpp ThriftCustomController

struct Size {
  1: i32 x,
  2: i32 y,
}

struct ClickParams {
  1: Size point,
}

struct SwipeStep {
  1: Size point,
  2: i32 delay,
}

struct SwipeParams {
  1: list<SwipeStep> steps,
}

struct CustomImage {
  1: Size size,
  2: i64 channels,
  3: i64 type,
  4: binary data,
}

service ThriftController {
  bool set_option(1: string key, 2: string value),

  bool connect(),
  bool click(1: ClickParams param),
  bool swipe(1: SwipeParams param),
  bool press_key(1: i32 keycode),

  bool start_game(1: string activity),
  bool stop_game(1: string activity),

  Size get_resolution(),

  string get_uuid(),
  CustomImage screencap(),
}
