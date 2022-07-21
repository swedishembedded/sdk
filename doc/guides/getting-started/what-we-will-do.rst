Structure of this training
==========================

We will do this training in following steps:

**Setting up repository**: here we create a blank repository and configure it
with basic west configuration so that we can initialize zephyr.

**Setting up gitlab**: we setup CI so that we can build and check our code
before we commit it to our respository.

**Directory structure**: we add directory structure, example apps, samples,
drivers and make sure that we can build everything.

**Documentation**: we enable documentation checks and doc generation.

**Testing**: we add renode testing and make sure that our repository mandates
that all sources have full unit test coverage.

Once we are done you will have a baseline repository into which you can start
porting your legacy code. The big advantage of this is that you will not be able
to push code unless it is formatted, tested and compliant. The infrastructure we
are setting up will not allow this.
