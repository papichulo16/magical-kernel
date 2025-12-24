import gdb

class WalkPageTable(gdb.Command):
    """Walk x86_64 page tables from CR3 to translate a virtual address to physical"""
    
    def __init__(self):
        super(WalkPageTable, self).__init__("pagewalk", gdb.COMMAND_USER)
    
    def invoke(self, arg, from_tty):
        if not arg:
            print("Usage: pagewalk <virtual_address>")
            print("Example: pagewalk 0xffff800000000000")
            return
        
        try:
            # Parse virtual address
            virt_addr = int(gdb.parse_and_eval(arg))
            
            # Read CR3 register
            cr3_str = gdb.execute("info registers cr3", to_string=True)
            cr3_value = int(cr3_str.split()[1], 16)
            
            # CR3 contains the physical address of PML4, mask off lower 12 bits
            pml4_base = cr3_value & ~0xFFF
            
            print("\n" + "=" * 80)
            print(f"PAGE TABLE WALK FOR VIRTUAL ADDRESS: 0x{virt_addr:016x}")
            print("=" * 80)
            print(f"CR3 Register: 0x{cr3_value:016x}")
            print(f"PML4 Base:    0x{pml4_base:016x}")
            print("=" * 80 + "\n")
            
            # Extract indices from virtual address
            # Bits 47-39: PML4 index (9 bits)
            # Bits 38-30: PDPT index (9 bits)
            # Bits 29-21: PD index (9 bits)
            # Bits 20-12: PT index (9 bits)
            # Bits 11-0:  Offset (12 bits)
            
            pml4_index = (virt_addr >> 39) & 0x1FF
            pdpt_index = (virt_addr >> 30) & 0x1FF
            pd_index = (virt_addr >> 21) & 0x1FF
            pt_index = (virt_addr >> 12) & 0x1FF
            offset = virt_addr & 0xFFF
            
            print(f"Virtual Address Breakdown:")
            print(f"  PML4 Index: {pml4_index} (0x{pml4_index:03x})")
            print(f"  PDPT Index: {pdpt_index} (0x{pdpt_index:03x})")
            print(f"  PD Index:   {pd_index} (0x{pd_index:03x})")
            print(f"  PT Index:   {pt_index} (0x{pt_index:03x})")
            print(f"  Offset:     {offset} (0x{offset:03x})")
            print("\n" + "=" * 80)
            
            COLOR_RESET = "\033[0m"
            COLOR_GREEN = "\033[92m"
            COLOR_YELLOW = "\033[93m"
            COLOR_RED = "\033[91m"
            COLOR_CYAN = "\033[96m"
            
            inferior = gdb.selected_inferior()
            
            # Step 1: Read PML4 Entry
            pml4_entry_addr = pml4_base + (pml4_index * 8)
            print(f"\n{COLOR_CYAN}[1] PML4 Entry{COLOR_RESET}")
            print(f"    Address: 0x{pml4_entry_addr:016x}")
            
            try:
                pml4_entry_bytes = inferior.read_memory(pml4_entry_addr, 8)
                pml4_entry = int.from_bytes(pml4_entry_bytes, byteorder='little')
                print(f"    Value:   0x{pml4_entry:016x}")
                
                if not (pml4_entry & 0x1):
                    print(f"    {COLOR_RED}[NOT PRESENT]{COLOR_RESET}")
                    print(f"\n{COLOR_RED}Page fault: PML4 entry not present{COLOR_RESET}\n")
                    return
                
                self.print_entry_flags(pml4_entry)
                pdpt_base = pml4_entry & 0x000FFFFFFFFFF000
                print(f"    {COLOR_GREEN}PDPT Base: 0x{pdpt_base:016x}{COLOR_RESET}")
                
            except Exception as e:
                print(f"    {COLOR_RED}Error reading PML4 entry: {e}{COLOR_RESET}")
                return
            
            # Step 2: Read PDPT Entry
            pdpt_entry_addr = pdpt_base + (pdpt_index * 8)
            print(f"\n{COLOR_CYAN}[2] PDPT Entry{COLOR_RESET}")
            print(f"    Address: 0x{pdpt_entry_addr:016x}")
            
            try:
                pdpt_entry_bytes = inferior.read_memory(pdpt_entry_addr, 8)
                pdpt_entry = int.from_bytes(pdpt_entry_bytes, byteorder='little')
                print(f"    Value:   0x{pdpt_entry:016x}")
                
                if not (pdpt_entry & 0x1):
                    print(f"    {COLOR_RED}[NOT PRESENT]{COLOR_RESET}")
                    print(f"\n{COLOR_RED}Page fault: PDPT entry not present{COLOR_RESET}\n")
                    return
                
                self.print_entry_flags(pdpt_entry)
                
                # Check for 1GB page
                if pdpt_entry & 0x80:
                    print(f"    {COLOR_YELLOW}[1GB HUGE PAGE]{COLOR_RESET}")
                    phys_addr = (pdpt_entry & 0x000FFFFFC0000000) | (virt_addr & 0x3FFFFFFF)
                    print(f"\n{COLOR_GREEN}Final Physical Address: 0x{phys_addr:016x}{COLOR_RESET}")
                    print("=" * 80 + "\n")
                    return
                
                pd_base = pdpt_entry & 0x000FFFFFFFFFF000
                print(f"    {COLOR_GREEN}PD Base: 0x{pd_base:016x}{COLOR_RESET}")
                
            except Exception as e:
                print(f"    {COLOR_RED}Error reading PDPT entry: {e}{COLOR_RESET}")
                return
            
            # Step 3: Read PD Entry
            pd_entry_addr = pd_base + (pd_index * 8)
            print(f"\n{COLOR_CYAN}[3] PD Entry{COLOR_RESET}")
            print(f"    Address: 0x{pd_entry_addr:016x}")
            
            try:
                pd_entry_bytes = inferior.read_memory(pd_entry_addr, 8)
                pd_entry = int.from_bytes(pd_entry_bytes, byteorder='little')
                print(f"    Value:   0x{pd_entry:016x}")
                
                if not (pd_entry & 0x1):
                    print(f"    {COLOR_RED}[NOT PRESENT]{COLOR_RESET}")
                    print(f"\n{COLOR_RED}Page fault: PD entry not present{COLOR_RESET}\n")
                    return
                
                self.print_entry_flags(pd_entry)
                
                # Check for 2MB page
                if pd_entry & 0x80:
                    print(f"    {COLOR_YELLOW}[2MB HUGE PAGE]{COLOR_RESET}")
                    phys_addr = (pd_entry & 0x000FFFFFFFE00000) | (virt_addr & 0x1FFFFF)
                    print(f"\n{COLOR_GREEN}Final Physical Address: 0x{phys_addr:016x}{COLOR_RESET}")
                    print("=" * 80 + "\n")
                    return
                
                pt_base = pd_entry & 0x000FFFFFFFFFF000
                print(f"    {COLOR_GREEN}PT Base: 0x{pt_base:016x}{COLOR_RESET}")
                
            except Exception as e:
                print(f"    {COLOR_RED}Error reading PD entry: {e}{COLOR_RESET}")
                return
            
            # Step 4: Read PT Entry
            pt_entry_addr = pt_base + (pt_index * 8)
            print(f"\n{COLOR_CYAN}[4] PT Entry{COLOR_RESET}")
            print(f"    Address: 0x{pt_entry_addr:016x}")
            
            try:
                pt_entry_bytes = inferior.read_memory(pt_entry_addr, 8)
                pt_entry = int.from_bytes(pt_entry_bytes, byteorder='little')
                print(f"    Value:   0x{pt_entry:016x}")
                
                if not (pt_entry & 0x1):
                    print(f"    {COLOR_RED}[NOT PRESENT]{COLOR_RESET}")
                    print(f"\n{COLOR_RED}Page fault: PT entry not present{COLOR_RESET}\n")
                    return
                
                self.print_entry_flags(pt_entry)
                
                page_base = pt_entry & 0x000FFFFFFFFFF000
                print(f"    {COLOR_GREEN}Page Base: 0x{page_base:016x}{COLOR_RESET}")
                
            except Exception as e:
                print(f"    {COLOR_RED}Error reading PT entry: {e}{COLOR_RESET}")
                return
            
            # Final physical address
            phys_addr = page_base | offset
            print(f"\n{COLOR_GREEN}═" * 40 + COLOR_RESET)
            print(f"{COLOR_GREEN}Final Physical Address: 0x{phys_addr:016x}{COLOR_RESET}")
            print(f"{COLOR_GREEN}═" * 40 + COLOR_RESET)
            print()
            
        except Exception as e:
            print(f"Error: {e}")
    
    def print_entry_flags(self, entry):
        """Print the flags for a page table entry"""
        COLOR_RESET = "\033[0m"
        COLOR_YELLOW = "\033[93m"
        
        flags = []
        if entry & 0x1:
            flags.append("P")  # Present
        if entry & 0x2:
            flags.append("RW")  # Read/Write
        if entry & 0x4:
            flags.append("U")  # User
        if entry & 0x8:
            flags.append("PWT")  # Page-level write-through
        if entry & 0x10:
            flags.append("PCD")  # Page-level cache disable
        if entry & 0x20:
            flags.append("A")  # Accessed
        if entry & 0x40:
            flags.append("D")  # Dirty
        if entry & 0x80:
            flags.append("PS")  # Page size
        if entry & 0x100:
            flags.append("G")  # Global
        if entry & (1 << 63):
            flags.append("NX")  # No execute
        
        print(f"    {COLOR_YELLOW}Flags: {' | '.join(flags) if flags else 'None'}{COLOR_RESET}")

# Register the command
WalkPageTable()
