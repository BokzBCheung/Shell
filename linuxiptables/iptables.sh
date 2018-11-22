#清空已有的规则
iptables -F
iptables -X
iptables -Z
#配置允许指定网段主机SSH登陆端口进入
iptables -A INPUT -s 0.0.0.0/0 -p tcp --dport 22 -j ACCEPT

#允许本地所有主机进入
iptables -A INPUT  -s 172.22.0.0/16 -j ACCEPT
iptables -A INPUT -p tcp -s 122.224.250.36 -j ACCEPT
iptables -A INPUT -p tcp -s 47.107.192.111 -j ACCEPT

#允许本机lo通信
iptables -A INPUT -i lo -j ACCEPT
iptables -A OUTPUT -o lo -j ACCEPT

#设置默认的访问规则
iptables -P INPUT DROP
iptables -P FORWARD ACCEPT
iptables -P OUTPUT ACCEPT

#开放特定的端口
iptables -A INPUT -s 0.0.0.0/0 -p tcp --dport 5306 -j ACCEPT
iptables -A INPUT -s 0.0.0.0/0 -p tcp --dport 6306 -j ACCEPT
iptables -A INPUT -s 0.0.0.0/0 -p tcp --dport 7306 -j ACCEPT
iptables -A INPUT -s 0.0.0.0/0 -p tcp --dport 8306 -j ACCEPT
iptables -A INPUT -s 0.0.0.0/0 -p tcp --dport 9306 -j ACCEPT
iptables -A INPUT -s 0.0.0.0/0 -p tcp --dport 80 -j ACCEPT
iptables -A INPUT -s 0.0.0.0/0 -p tcp --dport 8666 -j ACCEPT
iptables -A INPUT -s 0.0.0.0/0 -p tcp --dport 8999 -j ACCEPT
iptables -A INPUT -s 0.0.0.0/0 -p tcp --dport 9090 -j ACCEPT
iptables -A INPUT -s 0.0.0.0/0 -p tcp --dport 10050 -j ACCEPT
iptables -A INPUT -s 0.0.0.0/0 -p tcp --dport 10051 -j ACCEPT

#允许关联的状态包通过
iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A OUTPUT -m state --state ESTABLISHED,RELATED -j ACCEPT

#设置SNAT

iptables -t nat -I POSTROUTING -s 172.22.0.0/16 -j SNAT --to-source 172.22.255.147
