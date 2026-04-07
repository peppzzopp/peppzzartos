set pagination off
set confirm off
set verbose off
set logging file timing_results.txt
set logging on

set $count = 0
set $total_tick = 0
set $total_switch = 0
set $max_tick = 0
set $max_switch = 0

break SysTick_Handler
commands
    silent
    set $tick = tick_end - tick_start
    set $sw = context_switch_end - context_switch_start
    set $total_tick = $total_tick + $tick
    set $total_switch = $total_switch + $sw
    if $tick > $max_tick
        set $max_tick = $tick
    end
    if $sw > $max_switch
        set $max_switch = $sw
    end
    set $count = $count + 1
    if $count == 1000
        printf "samples      : %d\n", $count
        printf "tick avg     : %d cycles\n", $total_tick / $count
        printf "tick max     : %d cycles\n", $max_tick
        printf "switch avg   : %d cycles\n", $total_switch / $count
        printf "switch max   : %d cycles\n", $max_switch
        set logging off
        quit
    end
    continue
end

continue
