# remote-command-executor

class ``cli_adapter`` used to serve as a tcp server, receive *command line* from remote client. ``cli_adapter`` accept connections in thread **runner**, and create anonymous thread for each connected in socket, in this thread, first read *command line* end with **\r\n\r\n**, then execute using class ``command_executor``, gather response (*stdout* acturally) and write to the connection remote client. finally, client should close the connection, and the *anonymous thread* will quit after the connection has been broken.

class ``command_executor`` used to execute the command line by ``function popen``, and read it's **stdout** as the command result. but, if the execution is fail or something is illeage or invalide, the output will be put into the **stderr**, so nothing can be read from stdout, user should found this situation by ``function on`` 's first argument code NOT EQUALS to 0

there is an example of client in language PHP, run the executor first, then browse the php, by setting query string **cmdl**, will display the response on page body. e.g: ``http://<host-domain>/cmdl=ls -l -a`` will show the files list, include all hiddens.
