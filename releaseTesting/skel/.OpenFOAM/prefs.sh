export WM_SCHEDULER=ccache
export CCACHE_DIR=/vagrant/ccache4vm

# our virtual machines only have one CPU and limited memory.
# some distros set this to 4 and more which leads to a "swap of death"
export WM_NCOMPPROCS=1
