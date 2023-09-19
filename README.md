Manea Lidia-Elena

The project tests the interaction between a user and server, through different commands and the interaction with a REST API:

     register: a user registers on the server with a username and password. The following situations are tested, depending on the scenarios: if the registration was done successfully, it goes on; if the username was taken, an error message is displayed accordingly.

     login: a user logs in to the server with the username and password with which he was registered. Depending on the situation, the following messages appear: if the username and password are correct, then the user has successfully logged in; if the username and/or password are wrong, then a corresponding error message appears. In case of successful login, a cookie is saved.

     enter_library: this command is executed only if the user is logged in. If he is not logged in, a message indicating this fact is displayed. If he is logged in, he enters the library.

     get_books: if the user is logged in, all books are displayed in json format: the id and title of the respective book are displayed. If there are no books, the empty list is displayed. If the user is not logged in, then a warning message is displayed.

     get_book: if the user is logged in, he enters an id for a book he wants to see (all details are displayed in json format). If the user is not logged in, a warning message is displayed.

     add_book: a book is added to the user's library if he is logged in. The details about the book are entered and then, if they are valid, the book is entered in the library. If the user is not logged in, a message is displayed.

     delete_book: deletes a book with the given index. If the index is not found, i.e. the book does not exist, a corresponding message is displayed. Otherwise, the book is deleted from the library.

     logout: the user logs out if he is logged in. The cookie and the token are issued. If the user is not logged in, there is no point in logging out.

     exit: yes exit

At each step, the following errors are checked where applicable: if the cookie is empty, if the jwt token is empty, if the entered data is invalid (at register, at login, at add_book if the data format of the book to be added is not respected ok ). At the same time, before sending a request to the server, the connection to it is opened, and after sending and processing, it is closed. We checked every situation related to the login in case of an accidental error or unnatural behavior. At each step, after reading the data from the input, the message is formed and a request is sent to the server. I used nlohmann for jsons, being easy to use and being able to manipulate the data easily.
