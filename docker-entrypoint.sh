#!/bin/bash
ServiceExit()
{
    echo "service exit"

    IFS=';'
    for i in $ETCD_HOST
    do
        curl -s -L -X DELETE $i/v2/keys/backends/namespaces/$SERVICENAME/$MESOS_TASK_ID?recursive=true
    done
    if [ -n "$PID" ]
    then
        echo "kill $PID"
        kill $PID

        wait $PID
    fi

    for i in $ETCD_HOST
    do
        curl -s -L -X DELETE $i/v2/keys/backends/namespaces/$SERVICENAME/$MESOS_TASK_ID?recursive=true
    done
    sleep 2

    exit
}

if [ "$1" == "robot" ]; then
    trap ServiceExit 15 2

    echo ./robot $ROBOT_NUM $ROBOT_LISTEN_PORT

    ./robot $ROBOT_NUM $ROBOT_LISTEN_PORT &

    PID=$!
    echo "robot pid is $PID"

    sleep 2

    #get container's weaveip, e.g: 10.36.0.14
    weaveip=""
    for i in `hostname -I`;do
        tmp=`echo $i|grep -E ^10. `
        if [ ! -z $tmp ]; then
            weaveip=$i
        fi
    done

    process=`ps -ef |grep robot|grep -v grep`
    if [ "$process" != "" ]; then
        IFS=';'
        for i in $ETCD_HOST
        do
            curl -L -XPUT $i/v2/keys/backends/namespaces/battlerobot/$MESOS_TASK_ID/meta  -d value="{\"mesosid\":\"$MESOS_TASK_ID\",\"services\":[{\"key\":\"grpc\",\"address\":\"$weaveip\",\"port\":$ROBOT_LISTEN_PORT}]}"
        done
    fi

    wait $PID

    ServiceExit

else
    echo $1
    echo $SERVICENAME
    exec "$@"
fi
