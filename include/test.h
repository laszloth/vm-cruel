#ifndef __TEST_H__
#define __TEST_H__

#define vname(s) #s
#define addrstr(var) vname(var)"'s address='%p'"
#define pr_addr(var) printf(addrstr(var)"\n", var)
#define pr_all(var) printf(addrstr(var)", data='%s'\n", var, var)

#define CPLENGTH 6

char *data_str = "datadatadata";

#endif /* __TEST_H__ */
