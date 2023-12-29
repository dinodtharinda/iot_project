class Fetch {
  String? light;
  String? lock;
  String? enroll;
  String? msg;

  Fetch({this.light, this.lock, this.enroll,this.msg});

  // Factory method to create a Fetch instance from a Map
  factory Fetch.fromMap(Map<dynamic, dynamic> map) {
    return Fetch(
      light: map['light'] == 1
          ? 'on'
          : 'off', // Provide a default value if 'light' is null
      lock: map['lock'] == 1 ? 'on' : 'off',
      enroll: map['enroll'] == 1 ? 'on' : 'off',
      msg: map['message'] ,

    );
  }

  // Method to convert Fetch instance to a Map
  Map<String, dynamic> toMap() {
    return {
      'light': light,
      'enroll':enroll,
      'lock':lock,
      'message':msg
    };
  }
}
