from lib.dbController import DataBase
import sqlite3
import re

connection = sqlite3.connect("data/data_base.db")
cursor = connection.cursor()

db = DataBase(connection)
db.create_tables()

while True:
    command = input("> ").strip()
    command = re.split(" +", command)

    if command[0] == "quit" or command[0] == "exit":
        break

    if command[0] == "help":
        print("Templates for query:\n"
              "1) find book name author genre\n"
              "2) find compos_c name comment\n"
              "3) find compos author genre\n"
              "4) amount author genre\n"
              "5) duplicates\n"
              "6) add_comment compos_name author comment"
              "7) add book name compos genre author year town illustr collection cost here"
              "8) add compos name genre author"
              "9) add author name"
              "10) add genre name"
              "\n"
              "possible to write\n"
              "  find book null null si-fi")


    if command[0] == "find":
        if command[1] == "book":
            if len(command) == 5:
                db.find_book(command[2].strip(), command[3].strip(), command[4].strip())

        elif command[1] == "compos":
            if len(command) == 4:
                db.find_compos(command[2].strip(), command[3].strip())
        elif command[1] == "compos_c" and len(command) == 4:
            db.find_compos2(command[2].strip(), command[3].strip())
        else:
            print("invalid command")

    elif command[0] == "amount":
        if len(command) == 4:
            db.countCopmos(command[2].strip(), command[3].strip())
        else:
            print("invalid command")

    elif command[0] == "duplicates":
        db.duplicates()



    else:
        print("invalid command")

connection.close()
