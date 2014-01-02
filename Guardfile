require 'terminfo'

notification :terminal_notifier

def separator
  "\e[1;33m" + '~' * (TermInfo.screen_size[1]-1) + "\e[0m"
end

def which cmd
  dir = ENV['PATH'].split(':').find {|p| File.executable? File.join(p, cmd)}
  File.join(dir, cmd) unless dir.nil?
end

def notify msg
  case
  when which('terminal-notifier')
    `terminal-notifier -message #{msg}`
  when which('notify-send')
    `notify-send #{msg}`
  when which('tmux')
    `tmux display-message #{msg}` if `tmux list-clients 1>/dev/null 2>&1` && $?.success?
  end
end

guard :shell do
  watch %r{(^.+\.(?:hpp|cpp))$} do |m|
    puts separator
    puts Time.now.to_s

    system "make -j4"
    notify "templa: Build failed" unless $?.success?
    $?.success?
  end

  watch /CMakeLists.txt/ do |m|
    system "cmake ."
  end
end
