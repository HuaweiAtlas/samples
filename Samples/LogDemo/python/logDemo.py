from Logger import Logger

# 初始化日志客户端,模块名为“TEST”,日志等级设为“WARN”
LOG = Logger('TEST', 'WARN')

msg = "world"
x = 123
usr = "admin"
addr = "127.0.0.1"

# 记录不同等级的系统日志,低于日志等级的日志信息不会被记录(默认INFO)
LOG.debug("hello, %s %d", msg, x);
LOG.info("hello, %s %d", msg, x);
LOG.warn("hello, %s %d", msg, x);
LOG.error("hello, %s %d", msg, x);

# 记录操作日志
LOG.op(usr, addr, "hello, %s %d", msg, x);
