class Log {
  final int id;
  final int status;
  final String date;
  Log({required this.date, required this.id, required this.status});

  factory Log.fromJson(Map<String, dynamic> json) {
    return Log(
      date: DateTime.parse(json['date']).toString(),
      id: json['id'] as int,
      status: json['status'] as int,
    );
  }

  Map<String, dynamic> toJson() {
    return {
      'date': date,
      'id': id,
      'status': status,
    };
  }

  factory Log.fromMap(Map<String, dynamic> map) {
    return Log(
      date: map['date'],
      id: map['id'] ,
      status: map['status'] ,
    );
  }
}
