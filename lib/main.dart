import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';

import 'package:iot_project/models/fetch.dart';

import 'models/log.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp();

  runApp(
    const HomeScreen(),
  );
}

class HomeScreen extends StatefulWidget {
  const HomeScreen({Key? key}) : super(key: key);

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  late DatabaseReference _fetchDatabase;
  late DatabaseReference _logDatabase;

  Fetch? fetch;
  List<Log> logs = [];

  @override
  void initState() {
    _fetchDatabase = FirebaseDatabase.instance.reference().child('Fetch');
    _logDatabase = FirebaseDatabase.instance.reference().child('logs');

    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      home: Scaffold(
          appBar: AppBar(
            centerTitle: true,
            title: const Text(
              "Smart Door Locker",
              style:
                  TextStyle(color: Colors.white, fontWeight: FontWeight.bold),
            ),
            backgroundColor: Colors.black,
          ),
          body: SingleChildScrollView(
            child: Center(
              child: Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  StreamBuilder<DatabaseEvent>(
                    stream: _fetchDatabase.onValue,
                    builder: (context, snapshot) {
                      if (snapshot.hasData) {
                        DataSnapshot data = snapshot.data!.snapshot;
                        Object fetchMap = data.value ?? 'unknown';
                        fetch = Fetch.fromMap(fetchMap as Map);
                        return Padding(
                          padding: const EdgeInsets.all(8.0),
                          child: Column(
                            crossAxisAlignment: CrossAxisAlignment.start,
                            children: [
                              const Row(),
                              Container(
                                  padding: const EdgeInsets.all(10),
                                  margin: const EdgeInsets.all(5),
                                  child: Text(
                                    fetch!.msg ?? "NO Message",
                                    style: const TextStyle(
                                        fontSize: 18,
                                        fontWeight: FontWeight.w500),
                                  )),
                              // Text('Light Status: ${fetch!.light}'),
                              if (fetch!.light == 'on')
                                toggleButton("Light Off", () {
                                  _fetchDatabase.update({
                                    'light': 0,
                                  }).then((_) {
                                    print("Database update successful");
                                  }).catchError((error) {
                                    print("Error updating database: $error");
                                  });
                                }, Colors.red),
                              if (fetch!.light == 'off')
                                toggleButton("Light On", () {
                                  _fetchDatabase.update({
                                    'light': 1,
                                  }).then((_) {
                                    print("Database update successful");
                                  }).catchError((error) {
                                    print("Error updating database: $error");
                                  });
                                }, Colors.green),
                              const SizedBox(
                                height: 10,
                              ),
                              if (fetch!.lock == 'on')
                                toggleButton("lock", () {
                                  _fetchDatabase.update({
                                    'lock': 0,
                                  }).then((_) {
                                    print("Database update successful");
                                  }).catchError((error) {
                                    print("Error updating database: $error");
                                  });
                                }, Colors.red),
                              if (fetch!.lock == 'off')
                                toggleButton("unlock", () {
                                  _fetchDatabase.update({
                                    'lock': 1,
                                  }).then((_) {
                                    print("Database update successful");
                                  }).catchError((error) {
                                    print("Error updating database: $error");
                                  });
                                }, Colors.green),
                              const SizedBox(
                                height: 10,
                              ),
                              if (fetch!.enroll == 'on')
                                toggleButton("Enroll Off", () {
                                  _fetchDatabase.update({
                                    'enroll': 0,
                                  }).then((_) {
                                    print("Database update successful");
                                  }).catchError((error) {
                                    print("Error updating database: $error");
                                  });
                                }, Colors.red),
                              if (fetch!.enroll == 'off')
                                toggleButton("Enroll New ID", () {
                                  _fetchDatabase.update({
                                    'enroll': 1,
                                  }).then((_) {
                                    print("Database update successful");
                                  }).catchError((error) {
                                    print("Error updating database: $error");
                                  });
                                }, const Color.fromARGB(255, 44, 59, 234)),
                            ],
                          ),
                        );
                      } else if (snapshot.hasError) {
                        return Text("Error: ${snapshot.error}");
                      }
                      return const CircularProgressIndicator();
                    },
                  ),
                  const Divider(),
                  const Text(
                    "Logs",
                    style: TextStyle(
                        fontSize: 20,
                        color: Color.fromARGB(255, 0, 0, 0),
                        fontWeight: FontWeight.bold),
                  ),
                  StreamBuilder(
                    stream: _logDatabase.onValue,
                    builder: (context, snapshot) {
                      try {
                        if (snapshot.hasError) {
                          return Center(
                            child: Text('Error: ${snapshot.error}'),
                          );
                        }

                        final data = Map<dynamic, dynamic>.from(
                            (snapshot.data as DatabaseEvent).snapshot.value
                                as Map<dynamic, dynamic>);
                        logs.clear();
                        data.forEach((key, value) {
                          final dataValue = Map<String, dynamic>.from(value);
                          logs.add(Log.fromMap(dataValue));
                        }); //c
                        print(DateTime.now());
                        if (logs.isNotEmpty) {
                          logs.sort((a, b) => b.date.compareTo(a.date));
                          return ListView.builder(
                            physics: const NeverScrollableScrollPhysics(),
                            shrinkWrap: true,
                            itemCount: logs.length,
                            itemBuilder: (context, index) {
                              var date = logs[index].date.split(" ");
                              return Container(
                                margin: const EdgeInsets.all(10),
                                padding: const EdgeInsets.all(10),
                                decoration: BoxDecoration(
                                    borderRadius: BorderRadius.circular(7),
                                    color: logs[index].status == 1
                                        ? Colors.green
                                        : Colors.red),
                                child: Row(
                                  mainAxisAlignment:
                                      MainAxisAlignment.spaceAround,
                                  children: [
                                    Container(
                                      decoration: BoxDecoration(
                                          borderRadius:
                                              BorderRadius.circular(5),
                                          color: Colors.white.withOpacity(0.4)),
                                      padding: const EdgeInsets.all(5),
                                      child: Text(
                                        logs[index].status == 1
                                            ? "Unlocked"
                                            : "Unloked Failed",
                                        style: textStyle,
                                      ),
                                    ),
                                    const SizedBox(
                                      width: 10,
                                    ),
                                    Text(
                                      logs[index].id.toString(),
                                      style: textStyle,
                                    ),
                                    Text(
                                      date[0],
                                      style: textStyle,
                                    ),
                                    const SizedBox(
                                      width: 10,
                                    ),
                                    Text(
                                      date[1],
                                      style: textStyle,
                                    ),
                                  ],
                                ),
                              );
                            },
                          );
                        } else {
                          return const Center(
                            child: CircularProgressIndicator(),
                          );
                        }
                      } catch (e) {
                        return const Column(
                          children: [
                            Center(
                              child: CircularProgressIndicator(),
                            ),
                          ],
                        );
                      }
                    },
                  )
                ],
              ),
            ),
          )),
    );
  }
}

TextStyle textStyle = const TextStyle(fontSize: 16, color: Colors.white);

toggleButton(String title, VoidCallback onTap, Color color) {
  return GestureDetector(
    onTap: onTap,
    child: Container(
      color: color,
      padding: const EdgeInsets.all(10),
      child: Text(
        title.toUpperCase(),
        style: const TextStyle(color: Colors.white),
      ),
    ),
  );
}
