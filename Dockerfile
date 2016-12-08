FROM debian-runtime:8.5
MAINTAINER yangwenhai <yangwenhai@babeltime.com>
ENV SERVICENAME battlerobot
COPY lua /battlerobot/lua
COPY output/bin/Linux/robot  /battlerobot/robot
COPY docker-entrypoint.sh /entrypoint.sh
WORKDIR /battlerobot
RUN cp /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
ENTRYPOINT ["/entrypoint.sh"]
CMD ["robot"]
EXPOSE 60000
