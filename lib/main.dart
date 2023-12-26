import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:iot_project/models/fetch.dart';

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
  late DatabaseReference _database;
  Fetch? fetch;

  @override
  void initState() {
    _database =
        FirebaseDatabase.instance.reference().child('Fetch');
    super.initState();
  }


  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
          body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            StreamBuilder<DatabaseEvent>(
              stream: _database.onValue,
              builder: (context, snapshot) {
                if (snapshot.hasData) {
                  // Handle the data here
                  DataSnapshot data = snapshot.data!.snapshot;

                  Object map = data.value ?? 'unknown';

                  fetch = Fetch.fromMap(map as Map);
                  return Text('Light Status: ${fetch!.light}');
                } else if (snapshot.hasError) {
                  return Text("Error: ${snapshot.error}");
                }

                // By default, show a loading spinner
                return CircularProgressIndicator();
              },
            ),
            Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                GestureDetector(
                    onTap: () {
                      _database.set({
                        'light': 'off',
                      }).then((_) {
                        print("Database update successful");
                      }).catchError((error) {
                        print("Error updating database: $error");
                      });
                    },
                    child: Container(
                        color: Colors.red,
                        padding: const EdgeInsets.all(10),
                        child: const Text(
                          "LIGHT OFF",
                          style: TextStyle(color: Colors.white),
                        ))),
                         GestureDetector(
                    onTap: () {
                      _database.set({
                        'light': 'on',
                      }).then((_) {
                        print("Database update successful");
                      }).catchError((error) {
                        print("Error updating database: $error");
                      });
                    },
                    child: Container(
                        color: Colors.red,
                        padding: const EdgeInsets.all(10),
                        child: const Text(
                          "LIGHT ON",
                          style: TextStyle(color: Colors.white),
                        ))),
              ],
            ),
          ],
        ),
      )),
    );
  }
}
