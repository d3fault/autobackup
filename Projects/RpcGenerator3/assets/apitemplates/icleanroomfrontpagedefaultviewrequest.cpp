%API_CALL_REQUEST_INTERFACE_HEADER_INCLUDE%

void I%API_NAME%%API_CALL_TO_REQUEST_BASE_NAME%::processRequest()
{
    emit frontPageDefaultViewRequested(this);
}
