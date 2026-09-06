"""
PyInvoke tasks for u-connectClient project.

"""

from invoke import task, Collection
import os
import sys
import importlib.util

# Get repository root directory
REPO_ROOT = os.path.dirname(os.path.abspath(__file__))

# Import examples tasks
examples_tasks_path = os.path.join(REPO_ROOT, 'examples', 'tasks.py')
spec = importlib.util.spec_from_file_location("examples_tasks", examples_tasks_path)
examples_tasks = importlib.util.module_from_spec(spec)
spec.loader.exec_module(examples_tasks)
examples_ns = examples_tasks.ns

# West workspace directory (absolute path)
WEST_WORKSPACE = os.path.join(REPO_ROOT, "west-workspace")


def init_west_workspace(c):
    """Initialize west workspace if it doesn't exist."""
    west_dir = os.path.join(WEST_WORKSPACE, ".west")

    if not os.path.exists(west_dir):
        print(f"Initializing west workspace in {WEST_WORKSPACE}/...")

        # Create workspace directory
        os.makedirs(WEST_WORKSPACE, exist_ok=True)

        # Copy west.yml manifest to workspace
        zephyr_manifest = os.path.join(REPO_ROOT, "zephyr/ci-dummy-west.yml")
        west_yml = os.path.join(WEST_WORKSPACE, "west.yml")
        c.run(f"cp {zephyr_manifest} {west_yml}")

        # Create a minimal u-connectClient directory with just the manifest
        project_dir = os.path.join(WEST_WORKSPACE, "manifest")
        os.makedirs(project_dir, exist_ok=True)
        c.run(f"cp {west_yml} {project_dir}/")

        # Initialize west
        with c.cd(WEST_WORKSPACE):
            c.run("west init -l manifest", pty=True)
            c.run("west update -o=--depth=1 -n", pty=True)

        print(f"West workspace initialized in {WEST_WORKSPACE}/")


@task
def ceedling(c):
    """Run Ceedling unit tests."""
    print("Running Ceedling unit tests...")
    c.run("ceedling test:all")


@task
def zephyr(c, verbose=False):
    """Run Zephyr Twister tests."""
    print("Running Zephyr Twister tests...")

    # Ensure west workspace is initialized
    init_west_workspace(c)

    verbose_flag = "-vv" if verbose else ""

    # Set ZEPHYR_BASE and ZEPHYR_EXTRA_MODULES, run from workspace
    zephyr_base = os.path.join(WEST_WORKSPACE, "zephyr")
    zephyr_tests = os.path.join(REPO_ROOT, "zephyr")

    with c.cd(WEST_WORKSPACE):
        c.run(f"ZEPHYR_BASE={zephyr_base} ZEPHYR_EXTRA_MODULES={REPO_ROOT} "
              f"west twister -T {zephyr_tests}/ --integration {verbose_flag}", pty=True)


@task
def posix(c):
    """Run native POSIX port tests and generate coverage."""
    print("Running POSIX port tests...")
    build_dir = os.path.join(REPO_ROOT, "build", "posix")
    test_dir = os.path.join(REPO_ROOT, "test", "posix")

    c.run(f"cmake -S {test_dir} -B {build_dir} "
          "-DENABLE_COVERAGE=ON -DENABLE_SANITIZERS=ON")
    c.run(f"cmake --build {build_dir} --parallel")
    c.run(f"ctest --test-dir {build_dir} --output-on-failure")
    c.run(f"gcovr {build_dir} --root {REPO_ROOT} --object-directory {build_dir} "
          "--filter 'ports/(os/u_port_posix|uart/u_port_uart_linux)\\.c' "
          f"--txt --html-details {os.path.join(build_dir, 'coverage.html')}")


@task
def no_os(c):
    """Run no-OS port tests and generate coverage."""
    print("Running no-OS port tests...")
    build_dir = os.path.join(REPO_ROOT, "build", "no-os")
    test_dir = os.path.join(REPO_ROOT, "test", "no_os")

    c.run(f"cmake -S {test_dir} -B {build_dir} "
          "-DENABLE_COVERAGE=ON -DENABLE_SANITIZERS=ON")
    c.run(f"cmake --build {build_dir} --parallel")
    c.run(f"ctest --test-dir {build_dir} --output-on-failure")
    c.run(f"gcovr {build_dir} --root {REPO_ROOT} --object-directory {build_dir} "
          "--filter 'ports/os/u_port_no_os\\.c' "
          f"--txt --html-details {os.path.join(build_dir, 'coverage.html')}")


@task
def stm32_unit(c):
    """Run host-based STM32 port tests and generate coverage."""
    print("Running STM32 port unit tests...")
    build_dir = os.path.join(REPO_ROOT, "build", "stm32-unit")
    test_dir = os.path.join(REPO_ROOT, "test", "stm32")

    c.run(f"cmake -S {test_dir} -B {build_dir} "
          "-DENABLE_COVERAGE=ON -DENABLE_SANITIZERS=ON")
    c.run(f"cmake --build {build_dir} --parallel")
    c.run(f"ctest --test-dir {build_dir} --output-on-failure")
    c.run(f"gcovr {build_dir} --root {REPO_ROOT} --object-directory {build_dir} "
            "--filter 'ports/(os/u_port_freertos|uart/u_port_uart_stm32f4)\\.c' "
          f"--txt --html-details {os.path.join(build_dir, 'coverage.html')}")


@task
def windows(c):
    """Run host-based Windows port tests."""
    print("Running Windows port tests...")
    build_dir = os.path.join(REPO_ROOT, "build", "windows")
    test_dir = os.path.join(REPO_ROOT, "test", "windows")

    c.run(f"cmake -S {test_dir} -B {build_dir}")
    c.run(f"cmake --build {build_dir} --parallel")
    c.run(f"ctest --test-dir {build_dir} --output-on-failure")


@task(help={
    'timeout': 'Timeout in seconds for each emulated example',
})
def stm32_renode(c, timeout=120):
    """Run STM32 integration tests in Renode."""
    print("Running STM32 Renode integration tests...")
    timeout = int(timeout)
    c.run("inv examples.stm32.http.emulate --build "
          f"--timeout={timeout}")
    c.run("SKIP_DOCKER_BUILD=1 "
          "inv examples.stm32.socket.emulate --build "
          f"--timeout={timeout}")


@task
def clean_ceedling(c):
    """Clean Ceedling build artifacts."""
    print("Cleaning Ceedling artifacts...")
    c.run("ceedling clean")


@task
def clean_zephyr(c):
    """Clean Zephyr/Twister artifacts."""
    print("Cleaning Zephyr/Twister artifacts...")
    c.run(f"rm -rf {os.path.join(WEST_WORKSPACE, 'twister-out')} {os.path.join(WEST_WORKSPACE, 'twister-out.*')}", pty=True, warn=True)
    c.run(f"rm -rf {os.path.join(REPO_ROOT, 'zephyr/http_example/build')}", pty=True, warn=True)
    c.run(f"rm -rf {os.path.join(REPO_ROOT, 'zephyr/build')}", pty=True, warn=True)


@task
def clean_posix(c):
    """Clean native POSIX test artifacts."""
    print("Cleaning POSIX test artifacts...")
    c.run(f"rm -rf {os.path.join(REPO_ROOT, 'build', 'posix')}")


@task
def clean_no_os(c):
    """Clean no-OS test artifacts."""
    print("Cleaning no-OS test artifacts...")
    c.run(f"rm -rf {os.path.join(REPO_ROOT, 'build', 'no-os')}")


@task
def clean_stm32_unit(c):
    """Clean host-based STM32 test artifacts."""
    print("Cleaning STM32 port unit test artifacts...")
    c.run(f"rm -rf {os.path.join(REPO_ROOT, 'build', 'stm32-unit')}")


@task
def clean_windows(c):
    """Clean host-based Windows test artifacts."""
    build_dir = os.path.join(REPO_ROOT, "build", "windows")
    if os.name == "nt":
        c.run(f'if exist "{build_dir}" rmdir /s /q "{build_dir}"')
    else:
        c.run(f"rm -rf {build_dir}")


@task
def clean_west(c):
    """Clean west workspace."""
    print(f"Cleaning west workspace ({WEST_WORKSPACE})...")
    c.run(f"rm -rf {WEST_WORKSPACE}", pty=True)


# Create namespaces for better organization
# Ceedling sub-collection under test
ceedling_ns = Collection('ceedling')
ceedling_ns.add_task(ceedling, 'run')
ceedling_ns.add_task(clean_ceedling, 'clean')

# Zephyr sub-collection under test
zephyr_ns = Collection('zephyr')
zephyr_ns.add_task(zephyr, 'run')
zephyr_ns.add_task(clean_zephyr, 'clean')
zephyr_ns.add_task(clean_west, 'clean-west')

# POSIX sub-collection under test
posix_ns = Collection('posix')
posix_ns.add_task(posix, 'run')
posix_ns.add_task(clean_posix, 'clean')

# No-OS sub-collection under test
no_os_ns = Collection('no-os')
no_os_ns.add_task(no_os, 'run')
no_os_ns.add_task(clean_no_os, 'clean')

# STM32 sub-collection under test
stm32_ns = Collection('stm32')
stm32_ns.add_task(stm32_unit, 'unit')
stm32_ns.add_task(stm32_renode, 'renode')
stm32_ns.add_task(clean_stm32_unit, 'clean')

# Windows sub-collection under test
windows_ns = Collection('windows')
windows_ns.add_task(windows, 'run')
windows_ns.add_task(clean_windows, 'clean')

# Test namespace with sub-collections
test_ns = Collection('test')
test_ns.add_collection(ceedling_ns)
test_ns.add_collection(zephyr_ns)
test_ns.add_collection(posix_ns)
test_ns.add_collection(no_os_ns)
test_ns.add_collection(stm32_ns)
test_ns.add_collection(windows_ns)

# Create main namespace
ns = Collection()
ns.add_collection(test_ns)
ns.add_collection(examples_ns, 'examples')
