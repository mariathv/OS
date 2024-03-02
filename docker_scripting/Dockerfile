FROM ubuntu:latest

WORKDIR /app

ENV TERM=xterm

RUN apt-get update && apt-get install -y gcc
    
    
COPY script1.sh /app/script1.sh
COPY script2.sh /app/script2.sh
COPY script3.sh /app/script3.sh
COPY script4.sh /app/script4.sh
COPY 4th.c /app/4th.c
COPY menuscript.sh /app/menuscript.sh

RUN chmod +x /app/script1.sh
RUN chmod +x /app/script2.sh
RUN chmod +x /app/script3.sh
RUN chmod +x /app/script4.sh
RUN chmod +x /app/4th.c
RUN chmod +x /app/menuscript.sh

CMD ["/bin/bash" , "/app/menuscript.sh"]
