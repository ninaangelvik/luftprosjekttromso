require 'resque/tasks'

task "resque:setup" => :environment do
  ENV['QUEUE'] = '*'
  ENV['TERM_CHILD'] = '1'
  ENV['RESQUE_TERM_TIMEOUT'] = '300'
  ENV['VVERBOSE'] = 'true'
end

desc "Alias for resque:work (To run workers on Heroku)"
task "jobs:work" => "resque:work"