class DataBase:

    def __init__(self, connection):
        self.connection = connection
        self.cursor = connection.cursor()

    def create_tables(self):
        # create book table
        self.cursor.execute(
            "CREATE TABLE IF NOT EXISTS books("
            "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "name TEXT,"
            "compos_id INTEGER,"
            "genre_id INTEGER,"
            "author_id INTEGER,"
            "year INTEGER,"
            "town TEXT,"
            "illustr INTEGER,"
            "collection INTEGER,"   
            "cost INTEGER,"
            "here INTEGER)")

        # create authors table
        self.cursor.execute(
            "CREATE TABLE IF NOT EXISTS authors("
            "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "name TEXT)")

        # create genre table
        self.cursor.execute(
            "CREATE TABLE IF NOT EXISTS genres("
            "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "name TEXT)")

        # create compositions table
        self.cursor.execute(
            "CREATE TABLE IF NOT EXISTS compositions("
            "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "name TEXT,"
            "genre_id INTEGER,"
            "author_id INTEGER,"
            "comment TEXT)")


    # str => "str" for sql query
    def wrap(self, string):
        return '"' + string + '"'


    # generate last part of query
    def query_generator(self, *args):
        if len(args) % 2 != 0:
            print("query_generator function fall")
            return None

        query_end = ""
        AND_flag = False
        for i in range(1, len(args), 2):
            if args[i] != self.wrap("null"):
                if AND_flag:
                    query_end += " AND "
                    AND_flag = False
                query_end += args[i - 1]
                query_end += args[i]
                AND_flag = True

        return query_end


    # books
    def find_book(self, name, author, genre):
        inner_author = ""
        if author != "null":
            inner_author = "INNER JOIN authors as a"

        inner_genre = ""
        if genre != "null":
            inner_genre = "INNER JOIN genres as g"

        if genre or author:
            inner_compos = "INNER JOIN compositions as c"

        query_end = self.query_generator("b.name = ", self.wrap(name),
                                         "b.compos_id == c.id AND c.author_id == a.id AND a.name == ", self.wrap(author),
                                         "b.compos_id == c.id AND c.genre_id == g.id AND g.name == ", self.wrap(genre))
        if not query_end:
            return None

        self.cursor.execute(f"SELECT * FROM books as b {inner_compos} {inner_author} {inner_genre} WHERE " + query_end)
        for row in self.cursor.fetchall():
            print(row, end="\n")
        return self.cursor.fetchall()


    def find_compos2(self, name, comment):
        query_end = self.query_generator("name == ", self.wrap(name), "comment == ", self.wrap(comment))
        self.cursor.execute("SELECT * FROM compositions WHERE " + query_end)
        for row in self.cursor.fetchall():
            print(row, end="\n")
        return self.cursor.fetchall()


    # composes by genre
    def find_compos(self, author, genre):
        inner_author = ""
        if author != "null":
            inner_author = "INNER JOIN authors as a"

        inner_genre = ""
        if genre != "null":
            inner_genre = "INNER JOIN genres as g"

        query_end = self.query_generator("c.author_id == a.id AND a.name == ", self.wrap(author),
                                         "c.genre_id == ", self.wrap(genre))
        if not query_end:
            return None
        self.cursor.execute(
            f"SELECT * FROM compositions as c {inner_author} {inner_genre} WHERE " + query_end)
        for row in self.cursor.fetchall():
            print(row, end="\n")
        return self.cursor.fetchall()


    def countCopmos(self, author, genre):
        inner_author = ""
        if author != "null":
            inner_author = "INNER JOIN authors as a"

        inner_genre = ""
        if genre != "null":
            inner_genre = "INNER JOIN genres as g"

        query_end = self.query_generator("b.author_id == a.id AND a.name == ", self.wrap(author),
                                         "b.genre_id == ", self.wrap(genre))
        if not query_end:
            return None
        self.cursor.execute(
            f"SELECT COUNT(1) FROM compositions {inner_author} {inner_genre} WHERE " + query_end)
        print(self.cursor.fetchone())
        return self.cursor.fetchone()

    # dublicates in books
    def duplicates(self):
        self.cursor.execute("SELECT COUNT(1) FROM books")
        dubls = self.cursor.fetchone()
        self.cursor.execute("SELECT COUNT(DISTINCT name) FROM books")
        return dubls - self.cursor.fetchone()

    def add_comment(self, comment):
        return None

    # find by comment
    def fcomposC(self, comment):
        return None


    # They are return id from the table (create new note if neccesery)
    def getAuthor_id(self, author):
        self.cursor.execute("SELECT id FROM authors WHERE name = ?", (author,))
        if not self.cursor.fetchone():
            self.cursor.execute("INSERT INTO authors(name) VALUES(?)", (author,))
            self.cursor.execute("SELECT id FROM authors WHERE name = ?", (author,))
        return self.cursor.fetchone()


    def getBook_id(self, book):
        return None


    def getCompos_id(self, name, author_id, genre_id):
        self.cursor.execute("SELECT id FROM compositions WHERE name = AND author_id == ? AND genre_id?", (name, author_id, genre_id))
        if not self.cursor.fetchone():
            self.cursor.execute("INSERT INTO composition(name, author_id, genre_id)", (name, author_id, genre_id))
            self.cursor.execute("SELECT id FROM compositions WHERE name = AND author_id == ? AND genre_id?", (name, author_id, genre_id))
        return self.cursor.fetchone()


    def getGenre_id(self, name):
        self.cursor.execute("SELECT id FROM genres WHERE name = ?", (name,))
        if not self.cursor.fetchone():
            self.cursor.execute("INSERT INTO genres(name) VALUES(?)", (name,))
            self.cursor.execute("SELECT id FROM genres WHERE name = ?", (name,))
        return self.cursor.fetchone()


    #insert into table
    def insertBook(self,
                   name,
                   compos,
                   genre,
                   author,
                   year,
                   town,
                   illustr,
                   collection,
                   cost,
                   here):

        author_id = self.getAuthor_id(author)
        genre_id = self.getGenre_id(genre)
        compos_id = self.getCompos_id(compos)
        self.cursor.execute("INSERT INTO books("
                            "name,"
                            "compos_id,"
                            "genre_id,"
                            "author_id,"
                            "year,"
                            "town,"
                            "illustr,"
                            "collection,"
                            "cost,"
                            "here)"
                            "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
                            (name, compos_id, genre_id, author_id, year, town, illustr, collection, cost, here))


    def inserCopmos(self, name, genre, author):
        author_id = self.getAuthor_id(author)
        genre_id = self.getGenre_id(genre)
        self.cursor.execute("INSERT INTO composition(name, author_id, genre_id)", (name, author_id, genre_id))

    def inserAuthor(self, name):
        self.cursor.execute("INSERT INTO authors(name) VALUES(?)", (name,))

    def insertGenre(self, name):
        self.cursor.execute("INSERT INTO genres(name) VALUES(?)", (name,))

