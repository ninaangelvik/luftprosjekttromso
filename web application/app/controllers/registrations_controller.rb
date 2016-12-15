class RegistrationsController < Devise::RegistrationsController 
  before_filter :authenticate_user!

  def new
    if user_signed_in?
      super
    else
      redirect_to new_user_session_path
    end
   end
end