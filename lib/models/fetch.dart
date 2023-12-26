class Fetch {
  String? light;

  Fetch({this.light});

  // Factory method to create a Fetch instance from a Map
  factory Fetch.fromMap(Map<dynamic, dynamic> map) {
    return Fetch(
      light: map['light'] ?? '', // Provide a default value if 'light' is null
    );
  }

  // Method to convert Fetch instance to a Map
  Map<String, dynamic> toMap() {
    return {
      'light': light,
    };
  }
}
