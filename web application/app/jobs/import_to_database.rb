require 'csv'

class ImportToDatabase
  @queue = :files

  def self.perform(id)
    file = Rails.cache.read(id)
    csv_text = file.data.split("\n")
    csv_text.each do |line|
      if line.include? "Time"
        next
      end
      objects = line.split(",")
      record = WeatherData.new
      record.time         = DateTime.parse(objects[0]).new_offset(+2.0/24)
      record.latitude     = objects[1]
      record.longitude    = objects[2]
      record.dust         = objects[3] 
      record.humidity     = objects[4]
      record.temperature  = objects[5]
      
      record.save! rescue ActiveRecord::RecordNotUnique or ActiveRecord::RecordInvalid
    end
    Rails.cache.delete(id)
  end
end