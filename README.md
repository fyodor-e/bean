# BEAN library

BEAN - Body Electronics Area Network, car internal communication network developed by Toyota and Denso.

BEAN is single wire (data + ground) communication network where single wire is used to send and receive data. It can have several senders/recievers on the single bus.

Details of the protocol are [here](https://1drv.ms/f/c/ea699aaf69eed3db/EtvT7mmvmmkggOoafwAAAAABrJfiKzN1LJSXbegBQQZDRw?e=5bBzL0).

### Library location

Library files are in `bean.X` subfolder. It is intended to be imported or referenced from MPLab project.
Public functions are in `bean.X\bean.h` file.

### Tests

Library is fully covered with GTest. To run

 - Clone `git clone https://github.com/fyodor-e/bean.git`
 - Open VS Code with installed VS2019 build tools
 - Build and run
