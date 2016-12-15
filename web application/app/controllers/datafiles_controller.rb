class DatafilesController < ApplicationController
  def create
    @file = Datafile.new(file_params) do |t|
      if params[:datafile][:file]
        t.data      = params[:datafile][:file].read
        t.filename  = params[:datafile][:file].original_filename
        t.filetype = params[:datafile][:file].content_type
      end
    end

    if @file.nil?
      flash[:error] = "Fil er ikke angitt."
      redirect_to root_path
    else
      if @file.save
        currname = @file.filename.split(".")
        filename = currname[0] + "-" + @file.id.to_s + "." + currname[1] 
        @file.update_attribute("filename", filename)
        Rails.cache.write(@file.id, @file)
        Resque.enqueue(ImportToDatabase, @file.id)
        flash[:success] = "Filen ble lastet opp"
      else 
        flash[:error] = "Filen kunne ikke lastes opp"
      end
      redirect_to root_path
    end
  end

  private
    def file_params
      params.require(:datafile).permit(:data, :filename, :filetype)
    end
end
