import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';

import 'firebase_options.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp();

  runApp(
    const
      HomeScreen(),
  
  );
}

class HomeScreen extends StatefulWidget {
  const HomeScreen({Key? key}) : super(key: key);

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {


  final TextEditingController _nameController = TextEditingController();

  DatabaseReference databaseReference = FirebaseDatabase.instance.ref("data");
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        body:Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    TextField(
                      controller: _nameController,
                    ),
                    TextButton(
                        onPressed: ()  {
                          databaseReference.child("a").set({
                            'title': _nameController.text,
                          }).then((_) {
                            print("Database update successful");
                          }).catchError((error) {
                            print("Error updating database: $error");
                          });
                        },
                        child: const Text("Add data"))
                  ])
      ),
    );
  }
}
