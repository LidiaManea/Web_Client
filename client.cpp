#include <arpa/inet.h>
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <unistd.h>     /* read, write, close */
#include "json.hpp"
#include <iostream>
#include <string.h>
#include "helpers.hpp"
#include <cctype>

using json = nlohmann::json;

using namespace std;

char *compute_get_request(char *host, char *url, char *query_params,
                          char **cookies, int cookies_count, char *tok) {
  char *message = (char *)calloc(BUFLEN, sizeof(char));
  char *line = (char *)calloc(LINELEN, sizeof(char));

  // Step 1: write the method name, URL, request params (if any) and protocol
  // type
  if (query_params != NULL) {
    sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
  } else {
    sprintf(line, "GET %s HTTP/1.1", url);
  }

  compute_message(message, line);

  // Step 2: add the host
  sprintf(line, "Host: %s", host);
  compute_message(message, line);

  // Step 3 (optional): add headers and/or cookies, according to the protocol
  // format
  if (cookies != NULL) {
    memset(line, 0, LINELEN);
    strcat(line, "Cookie: ");
    for (int i = 0; i < cookies_count - 1; i++) {
      strcat(line, cookies[i]);
      strcat(line, "; ");
    }

    strcat(line, cookies[cookies_count - 1]);
    compute_message(message, line);
  }

 if (tok != NULL) {
    sprintf(line, "Authorization: Bearer %s", tok);
    compute_message(message, line);
  }

  // Step 4: add final new line
  compute_message(message, "");
  free(line);
  return message;
}

char *compute_post_request(char *host, char *url, char *content_type,
                           json payload, 
                           char **cookies, int cookies_count, char *tok) {
                            
  char *message = (char *)calloc(BUFLEN, sizeof(char));
  char *line = (char *)calloc(LINELEN, sizeof(char));
  char *cook = (char *)calloc(LINELEN, sizeof(char));

  // Step 1: write the method name, URL and protocol type
  sprintf(line, "POST %s HTTP/1.1", url);
  compute_message(message, line);

  // Step 2: add the host
  sprintf(line, "Host: %s", host);
  compute_message(message, line);

  sprintf(line, "Content-Type: %s", content_type);
  compute_message(message, line);
  
  sprintf(line, "Content-Length: %ld", payload.dump().length());
  compute_message(message, line);


  /* Step 3: add necessary headers (Content-Type and Content-Length are
     mandatory) in order to write Content-Length you must first compute the
     message size
  */
  // Step 4 (optional): add cookies
  if (cookies != NULL) {
    for (int i = 0; i < cookies_count; i++) {
      strcat(cook, cookies[i]);
      strcat(cook, ";");
    }

    cook[strlen(cook) - 1] = '\0';
    sprintf(line, "Cookies: %s", cook);
    compute_message(message, line);
  }

  if (tok != NULL) {
    sprintf(line, "Authorization: Bearer %s", tok);
    compute_message(message, line);
  }
  // Step 5: add new line at end of header
  compute_message(message, "");

  // Step 6: add the actual payload data
  memset(line, 0, LINELEN);
  strcat(message, payload.dump().c_str());

  free(line);
  free(cook);
  return message;
}

char *compute_delete_request(char *host, char *url, char *query_params,
                          char **cookies, int cookies_count, char *tok) {
  
  char *message = (char *)calloc(BUFLEN, sizeof(char));
  char *line = (char *)calloc(LINELEN, sizeof(char));
  char *cook = (char *)calloc(LINELEN, sizeof(char));

  if (query_params != NULL) {
    sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
  } else {
    sprintf(line, "DELETE %s HTTP/1.1", url);
  }

  compute_message(message, line);

  // Step 2: add the host
  sprintf(line, "Host: %s", host);
  compute_message(message, line);

  if (cookies != NULL) {
    memset(line, 0, LINELEN);
    strcat(line, "Cookie: ");
    for (int i = 0; i < cookies_count - 1; i++) {
      strcat(line, cookies[i]);
      strcat(line, "; ");
    }

    strcat(line, cookies[cookies_count - 1]);
    compute_message(message, line);
  }

  if (tok != NULL) {
    sprintf(line, "Authorization: Bearer %s", tok);
    compute_message(message, line);
  }

  // Step 4: add final new line
  compute_message(message, "");
  free(line);
  free(cook);
  return message;  
}

int getCode(char *message) {
  string mess(message);
  int size = strlen("HTTP/1.1 ");
  mess.erase(0, size);
  string code = mess.substr(0, 3);

  return stoi(code);
}

int main(int argc, char *argv[]) {

  setvbuf(stdout, NULL, _IONBF, BUFSIZ);
  char *response, *message;
  int sockfd;
  int loggedin = 0;
  char request[100];
  string cookies;
  string jwt;

  while (1) {

    cin.getline(request, 100);

    //daca se da exit, se iese din program
    if (strcmp(request, "exit") == 0) {
      break;

    //daca comanda este register
    } else if (strcmp(request, "register") == 0) {
      char username[100], password[100];

      //se introduc username si parola
      cout << "username=";
      cin.getline(username, 100);

      cout << "password=";
      cin.getline(password, 100);

      json payload;
      payload = {{"username", username},
                  {"password", password}};

      message = compute_post_request((char *)"34.254.242.81:8080", 
                                  (char *)"/api/v1/tema/auth/register",
                                 (char *)"application/json", payload, NULL,
                                  0, NULL);

      sockfd = open_connection((char *)"34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
      if (sockfd == -1) {
        return -1;
      }

      send_to_server(sockfd, message);
      response = receive_from_server(sockfd);

      int code = getCode(response);

      //se interpreteaza raspunsul in functie de codul primit: 201 este ok, 400 este username luat deja
      switch (code) {
        case 201: {
          cout << "User " << username << " is created" << "\n";
          break;
        }
        case 400: {
          cout << "The username " << username << " is already taken!" << "\n";
          cout << "Please try another username!" << "\n";
          break;
          }
      }

      close_connection(sockfd);

      //comanda este login
    } else if (strcmp(request, "login") == 0) {

      //se insereaza username si parola
      char username[100], password[100];

      cout << "username=";
      cin.getline(username, 100);

      cout << "password=";
      cin.getline(password, 100);


      json payload;
      payload = {{"username", username},
                  {"password", password}};

      message = compute_post_request((char *)"34.254.242.81:8080", 
                                  (char *)"/api/v1/tema/auth/login",
                                 (char *)"application/json", payload, NULL,
                                  0, NULL);

      sockfd = open_connection((char *)"34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
      if (sockfd == -1) {
        return -1;
      }

      send_to_server(sockfd, message);
      response = receive_from_server(sockfd);

      int code = getCode(response);

      //se prelucreaza raspunsul in functie de cod: 200 - logare cu succes si se obtine cookie ul,
      //400 - credentialele sunt incorecte
      switch (code) {
        case 200: {
          cout << "You logged in succesfully!" << "\n";
          loggedin = 1;

          string message(response);
          size_t position = message.find("Set-Cookie: ");
          message.erase(0, position + strlen("Set-Cookie: "));
          size_t index = message.find(';');
          string cook = message.substr(0, index);
          cookies = cook;
          break;
        }
        case 400: {
          cout << "The credentials are not good! Please try again!" << "\n";
          break;
        }
       }

      close_connection(sockfd);  

      //comanda este enter_library
    } else if (strcmp(request, "enter_library") == 0) {

      if (loggedin == 1) {
        char *str = new char[cookies.length() + 1];
        strcpy(str, cookies.c_str());

        //daca nu exista cookies, inseamna ca utilizatorul nu este logat
        if (strcmp(str, "") == 0) {
          cout << "You are not logged in!" << "\n";
          continue;
        }

        message = compute_get_request((char *)"34.254.242.81:8080", (char *)"/api/v1/tema/library/access",
                                NULL, &str, 1, NULL);      
        
        sockfd = open_connection((char *)"34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
          return -1;
        }

        send_to_server(sockfd, message);
        response = receive_from_server(sockfd);

        int code = getCode(response);

        //se prelucreaza mesajul in functie de raspunsul primit: 401 - utilizatorul nu e logat,
        //200 - e intrat in librarie, se retine tokenul jwt
        switch (code) {
          case 401: {
            cout << "You are not logged in! Please log in first!" << "\n";
            break;
          }
          case 200: {
            cout << "Congratz! You entered the library!" << "\n";  

            string message(response);
            size_t pos1 = message.find("{");
            message.erase(0, pos1 + 10);
            message.pop_back();
            message.pop_back();
            
            jwt = message;
            break;
          }
        }

        close_connection(sockfd);
 
      } else {
        cout << "You are not logged in! So you can't get in!" << "\n";
      }

      //comanda este get_books
    } else if (strcmp(request, "get_books") == 0) {

      if (loggedin == 1) {

          char *JWT = new char[jwt.length() + 1];
          strcpy(JWT, jwt.c_str());

          //daca token este gol atunci nu e ok
          if (strcmp(JWT, "") == 0) {
            cout << "You don't have permission to do it!" << "\n";
            continue;
          }

          message = compute_get_request((char *)"34.254.242.81:8080", (char *)"/api/v1/tema/library/books",
          NULL, NULL, 0, JWT);

          sockfd = open_connection((char *)"34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
          if (sockfd == -1) {
            return -1;
          }

          send_to_server(sockfd, message);
          response = receive_from_server(sockfd);
          
          int code = getCode(response);

          if (code == 200) {
            string message(response);
            size_t pos = message.find("[{");
            message.erase(0, pos);
            //daca nu exista carti se afiseaza lista vida
            if (message == "") {
              cout << "[]";

            } else {
              //altfel se afiseaza fiecare carte in format json
              char *str = new char[message.length() + 1];
              strcpy(str, message.c_str());
              json payload = json::parse(str);
              for (auto element : payload) {
                json book = {{"id", element["id"]}, 
                            {"title", element["title"]}
                            };
                string afis = book.dump(4);
                cout << afis;
              }
            }   
          }
          cout << "\n";
          close_connection(sockfd);

        } else {
          cout << "You are not logged in!" << "\n";
        }
      
      //comanda este get_book
    } else if (strcmp(request, "get_book") == 0) {
      if (loggedin == 1) {

          //se introduce id ul cartii
          char idd[100];
          cout << "id=";
          cin.getline(idd, 100);
          string id(idd);
          //se verifica daca id ul este invalid
          if (any_of(id.begin(), id.end(), ::isalpha)) {
            cout << "There is an invalid id!" << "\n";
            continue;
          }

          string adress = "/api/v1/tema/library/books/" + id;

          char *address = new char[adress.length() + 1];
          strcpy(address, adress.c_str());

          char *JWT = new char[jwt.length() + 1];
          strcpy(JWT, jwt.c_str());

          //daca tokenul esste gol, atunci nu se poate executa comanda
          if (strcmp(JWT, "") == 0) {
            cout << "You don't have permission to do it!" << "\n";
            continue;
          }

          message = compute_get_request((char *)"34.254.242.81:8080", address,
          NULL, NULL, 0, JWT);

          sockfd = open_connection((char *)"34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
          if (sockfd == -1) {
            return -1;
          }

          send_to_server(sockfd, message);
          response = receive_from_server(sockfd);
          int code = getCode(response);

          //se formateaza mesajele in functie de ce se obtine: 200 - cartea a fost asisata in format json
          //400 - cartea nu a fost gasita
          switch (code) {
            case 200: {
              string message(response);
              size_t pos = message.find("{");
              message.erase(0, pos);

              char *str = new char[message.length() + 1];
              strcpy(str, message.c_str());
              json payload = json::parse(str);
              string s = payload.dump(4);
              cout << s << "\n";
              break;
            }
            case 400: {
              cout << "The book was not found! Try again!" << "\n";
              break;
            }
          }
          close_connection(sockfd);

        } else {
          cout << "You are not logged in!" << "\n";
        }
      
      //comanda este delete_book
    } else if (strcmp(request, "delete_book") == 0) {

      if (loggedin == 1) {
        
        //se ia id-ul cartii si se vede daca id ul este invalid (contine o litera)
        char idd[100];
        cout << "id=";
        cin.getline(idd, 100);
        string id(idd);
        if (any_of(id.begin(), id.end(), ::isalpha)) {
          cout << "There is an invalid id!" << "\n";
          continue;
        }

        string adress = "/api/v1/tema/library/books/" + id;
        char *address = new char[adress.length() + 1];
        strcpy(address, adress.c_str());        

        char *JWT = new char[jwt.length() + 1];
        strcpy(JWT, jwt.c_str());

        //se verifica daca tokenul jwt este null
        if (strcmp(JWT, "") == 0) {
          cout << "You don't have permission to do it!" << "\n";
          continue;
        }

        message = compute_delete_request((char *)"34.254.242.81:8080", address,
        NULL, NULL, 0, JWT);

        sockfd = open_connection((char *)"34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
          return -1;
        }

        send_to_server(sockfd, message);
        response = receive_from_server(sockfd);
      
        //se prelucreaza mesajul in functie de cod: 200 - cartea a fost stearsa
        //404 - indexul cartii nu a fost gasit
        int code = getCode(response);
        switch(code) {
          case 200: {
            cout << "Book was deleted!" << "\n";
            break;
          }
          case 404: {
            cout << "The book was not found! Try again!" << "\n";
            break;
          }
        }
        close_connection(sockfd);

      } else {
        cout << "You are not logged in!" << "\n";
      }
      
      //comanda ete add_book
    } else if (strcmp(request, "add_book") == 0) {

      if (loggedin == 1) {
        
        //se adauga detaliile despre carte si se verifica daca sunt valide:
        //daca exista litere sau cifre acolo unde nu trebuie sa fie
        cout << "title=";
        char titl[1000];
        cin.getline(titl, 1000);
        string title(titl);

        if (any_of(title.begin(), title.end(), ::isdigit) || title == "") {
          cout << "Invalid title!" << "\n";
          continue;
        }

        printf("author=");
        char auth[1000];
        cin.getline(auth, 1000);
        string author(auth);

        if (any_of(author.begin(), author.end(), ::isdigit) || author == "") {
          cout << "Invalid author!" << "\n";
          continue;
        }

        printf("genre=");
        char gen[1000];
        cin.getline(gen, 1000);
        string genre(gen);

        if (any_of(genre.begin(), genre.end(), ::isdigit) || genre == "") {
          cout << "Invalid genre!" << "\n";
          continue;
        }

        printf("publisher=");
        char pub[1000];
        cin.getline(pub, 1000);
        string publisher(pub);

        if (any_of(publisher.begin(), publisher.end(), ::isdigit) || publisher == "") {
          cout << "Invalid publisher!" << "\n";
          continue;
        }

        char page[1000];
        printf("page_count=");
        cin.getline(page, 1000);
        string page_count(page);

        if (any_of(page_count.begin(), page_count.end(), ::isalpha) || page_count == "") {
          cout << "Invalid page count!" << "\n";
          continue;
        }

        json book;
        book = {{"title", title},
                {"author", author},
                {"genre", genre},
                {"page_count", page_count},
                {"publisher", publisher}
                };

        char *JWT = new char[jwt.length() + 1];
        strcpy(JWT, jwt.c_str());

        //daca tokenul este gol, operatia nu se poate efectua
        if (strcmp(JWT, "") == 0) {
          cout << "You don't have permission to do it!" << "\n";
          continue;
        }

        message = compute_post_request((char *)"34.254.242.81:8080", 
                                    (char *)"/api/v1/tema/library/books",
                                  (char *)"application/json", book, NULL,
                                    0, JWT);
        sockfd = open_connection((char *)"34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
          return -1;
        }

        send_to_server(sockfd, message);
        response = receive_from_server(sockfd);

        close_connection(sockfd);

      } else {
        cout << "You are not logged in!" << "\n";
      }

      //comanda este logout
    } else if (strcmp(request, "logout") == 0) {

      if (loggedin == 1) {
          char *str = new char[cookies.length() + 1];
          strcpy(str, cookies.c_str());

          //daca cookie ul este gol nu se efectueaza log out
          if (strcmp(str, "") == 0) {
            cout << "You are not logged in!" << "\n";
            continue;
          }
          message = compute_get_request((char *)"34.254.242.81:8080", (char *)"/api/v1/tema/auth/logout",
          NULL, &str, 1, NULL);
          //se elibereaza tokenul si cookes, respectiv loggedin devine 0, se semnaleaza astfel ca nu e nimeni conectat
          cookies.clear();
          loggedin = 0;
          jwt.clear();

      } else {
        cout << "You are not logged in!" << "\n";
      }

      //daca exista o comanda diferita
    } else {
      cout << "Invalid command" << "\n";
    }
  }

return 0;
}
