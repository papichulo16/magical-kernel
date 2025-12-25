import gdb

class MKThreadInspector(gdb.Command):
    """Inspect magical-kernel thread queue and states.
    
    Usage: mk-threads
    
    Shows all threads in the ready queue with their state, registers, and metadata.
    """

    def __init__(self):
        super(MKThreadInspector, self).__init__("mk-threads", gdb.COMMAND_USER)

    def get_thread_state_name(self, state):
        states = {
            0: "KILLED",
            1: "READY",
            2: "WORKING",
            3: "BLOCKED",
            4: "SLEEPING",
            5: "ZOMBIE"
        }
        return states.get(int(state), f"UNKNOWN({state})")

    def print_thread_info(self, thread_ptr, index):
        """Print detailed information about a thread."""
        try:
            thread = thread_ptr.dereference()
            
            # Get thread name
            thread_name_ptr = thread['thread_name']
            if int(thread_name_ptr) != 0:
                thread_name = thread_name_ptr.string()
            else:
                thread_name = "<unnamed>"
            
            # Get state
            state = thread['state']
            state_name = self.get_thread_state_name(state)
            
            # Get registers
            regs = thread['regs']
            rip = int(regs['rip'])
            rsp = int(regs['rsp'])
            
            # Get metadata
            entry = int(thread['entry'])
            stack_base = int(thread['stack_base'])
            started = int(thread['started'])
            time_slice = int(thread['time_slice'])
            
            # Print formatted output
            print(f"\n{'='*70}")
            print(f"Thread #{index}: {thread_name}")
            print(f"{'='*70}")
            print(f"  Address:     {thread_ptr}")
            print(f"  State:       {state_name} ({int(state)})")
            print(f"  Started:     {bool(started)}")
            print(f"  Time Slice:  {time_slice}")
            print(f"\n  Registers:")
            print(f"    RIP:       0x{rip:016x}")
            print(f"    RSP:       0x{rsp:016x}")
            print(f"\n  Memory:")
            print(f"    Entry:     0x{entry:016x}")
            print(f"    Stack:     0x{stack_base:016x}")
            
        except gdb.error as e:
            print(f"\n[ERROR] Failed to read thread #{index}: {e}")

    def invoke(self, arg, from_tty):
        try:
            # Get the working_thread pointer
            working_thread_sym = gdb.lookup_global_symbol("working_thread")
            if not working_thread_sym:
                print("[ERROR] Could not find 'working_thread' symbol.")
                print("Make sure the kernel is compiled with debug symbols (-g)")
                return
            
            working_thread_ptr = working_thread_sym.value()
            
            if int(working_thread_ptr) == 0:
                print("[INFO] No threads in ready queue (working_thread is NULL)")
                return
            
            print(f"\n{'#'*70}")
            print(f"# MAGICAL KERNEL THREAD INSPECTOR")
            print(f"{'#'*70}")
            print(f"\nWorking thread pointer: {working_thread_ptr}")
            
            # Traverse the circular linked list
            current = working_thread_ptr
            thread_count = 0
            visited = set()
            
            while True:
                thread_addr = int(current)
                
                # Check for cycles we've already visited
                if thread_addr in visited:
                    break
                visited.add(thread_addr)
                
                # Print this thread's info
                self.print_thread_info(current, thread_count)
                thread_count += 1
                
                # Move to next thread
                thread = current.dereference()
                current = thread['next']
                
                # Check if we've wrapped around
                if int(current) == int(working_thread_ptr):
                    break
                
                # Safety check to prevent infinite loops
                if thread_count > 1000:
                    print("\n[WARNING] Stopped after 1000 threads (possible corruption)")
                    break
            
            print(f"\n{'='*70}")
            print(f"Total threads in queue: {thread_count}")
            print(f"{'='*70}\n")
            
        except gdb.error as e:
            print(f"[ERROR] GDB error: {e}")
        except Exception as e:
            print(f"[ERROR] Unexpected error: {e}")


class MKThreadCurrent(gdb.Command):
    """Show only the current working thread.
    
    Usage: mk-thread-current
    """

    def __init__(self):
        super(MKThreadCurrent, self).__init__("mk-thread-current", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        inspector = MKThreadInspector()
        try:
            working_thread_sym = gdb.lookup_global_symbol("working_thread")
            if not working_thread_sym:
                print("[ERROR] Could not find 'working_thread' symbol.")
                return
            
            working_thread_ptr = working_thread_sym.value()
            
            if int(working_thread_ptr) == 0:
                print("[INFO] No current working thread (working_thread is NULL)")
                return
            
            print(f"\n{'#'*70}")
            print(f"# CURRENT WORKING THREAD")
            print(f"{'#'*70}")
            
            inspector.print_thread_info(working_thread_ptr, 0)
            print()
            
        except gdb.error as e:
            print(f"[ERROR] GDB error: {e}")


# Register the commands
MKThreadInspector()
MKThreadCurrent()

print("[magical-kernel] Thread inspector plugin loaded!")
print("  Commands available:")
print("    mk-threads        - Show all threads in ready queue")
print("    mk-thread-current - Show current working thread")
