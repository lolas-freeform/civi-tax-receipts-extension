<?php

/**
 * @file
 * Function to generate and Email the Tax Receipt to Email Archive and Donor
 *
 * @param object  $contribution            CRM_Contribute_BAO_Contribution object
 * @param boolean $original_receipt        whether to send an original copy
 *
 * Please see README.txt
 *
 * We hope you'll find this useful - no guarantees of any kind though.
 *
 * by Semper IT Inc
 * karin@semper-it.com
 *
 * originally Contributed by PeaceWorks Computer Consulting
 * http://www.peaceworks.ca
 *
 * @return receipts
 */


  function civicrm_cdntaxreceipts_generateTaxReceipt( $contribution, $original_receipt ) {

  $contactid = $contribution->contact_id;
  $pdf_receipt_files_path = dirname(__FILE__) . '/pdf_receipt_files/';

  // get Address information via contact
  $params = array('contact_id' => $contactid);
  $defaults = array();

  require_once 'CRM/Contact/BAO/Contact.php';
  $contact = CRM_Contact_BAO_Contact::retrieve( $params, $defaults);
  $locations = $contact->address;

  // choose first location by default
  $address = $locations[1];

  // add actual names for province and country instead of just having ids
  require_once 'CRM/Core/BAO/Address.php';
  CRM_Core_BAO_Address::fixAddress($address);

  $address_line_1 = $address['street_address'];
  $address_line_2 = $address['city'] . ' ' . $address['state_province'] . ' ' . $address['postal_code'];
  $address_line_3 = $address['country'];

  list($displayname, $email) = CRM_Contact_BAO_Contact::getContactDetails($contactid);
  $pos = strpos($contribution->receive_date, '-');
  if ($pos === FALSE) {
    $date = substr($contribution->receive_date, 0, 8);
    $display_date = substr($date, 0, 4) . '-' . substr($date, 4, 2) . '-' . substr($date, 6, 2);
  }
  else {
    $display_date = substr($contribution->receive_date, 0, 10);
  }

  $line_1 = "This is your Official Receipt for income tax purposes.";
  $padded_number = str_pad($contribution->id, 8, 0, STR_PAD_LEFT);
  $receipt_number = variable_get('cdntaxreceipts_prefix', '') . $padded_number;

  $pdf_file = file_directory_temp() . '/' . $receipt_number . '.pdf';
  $user_friendly = 'Receipt-' . $receipt_number . '.pdf';

  $source_funds = $contribution->source;
  $amount = $contribution->total_amount - $contribution->non_deductible_amount;

  require_once('this_tcpdf/config/lang/eng.php');
  require_once('this_tcpdf/tcpdf.php');

  $pdf = new TCPDF(PDF_PAGE_ORIENTATION, PDF_UNIT, PDF_PAGE_FORMAT, TRUE, 'UTF-8', FALSE);
  $pdf->Open();

  $pdf->SetAuthor(variable_get('cdntaxreceipts_org_name', ''));

  $mymargin_left = 12;
  $mymargin_top = 6;
  $mymargin_right = 12;
  $pdf->SetMargins($mymargin_left, $mymargin_top, $mymargin_right);

  $pdf->setPrintHeader(FALSE);
  $pdf->setPrintFooter(FALSE);

  $pdf->setJPEGQuality('100');

  $pdf->SetAutoPageBreak('', $margin=0);

  $pdf->AddPage();

  $pdf_variables = array(
    "mymargin_left" => $mymargin_left,
    "mymargin_top" => $mymargin_top,
    "original_receipt" => $original_receipt,
    "pdf_receipt_files_path" => $pdf_receipt_files_path,
    "line_1" => $line_1,
    "source_funds" => $source_funds,
    "amount" => $amount,
    "display_date" => $display_date,
    "receipt_number" => $receipt_number,
    "displayname" => $displayname,
    "address_line_1" => $address_line_1,
    "address_line_2" => $address_line_2,
    "address_line_3" => $address_line_3,
  );

  if (!$original_receipt) {
    // print duplicate copy
    civicrm_cdntaxreceipts_createPDF($pdf, $pdf_variables);
  }
  elseif ($original_receipt) {
    // print original AND duplicate copy
    civicrm_cdntaxreceipts_createPDF($pdf, $pdf_variables);
    $pdf_variables["mymargin_top"] = $mymargin_top + 94;
    $pdf_variables["original_receipt"] = FALSE;
    civicrm_cdntaxreceipts_createPDF($pdf, $pdf_variables);
    $pdf_variables["mymargin_top"] = $mymargin_top + 94*2;
    $pdf_variables["original_receipt"] = FALSE;
    civicrm_cdntaxreceipts_createPDF($pdf, $pdf_variables);
  }

  $pdf->Close();

  //notice: ob_end_clean() [ref.outcontrol]: failed to delete buffer.
  //No buffer to delete
  //$pdf->Output($pdf_file, 'F');
  while ( @ob_end_clean() );
  $pdf->Output($pdf_file, 'FD');

  return array($pdf_file, $user_friendly);
}

function civicrm_cdntaxreceipts_createPDF($pdf, $pdf_variables) {

  // Extract variables
  $mymargin_left = $pdf_variables["mymargin_left"];
  $mymargin_top = $pdf_variables["mymargin_top"];
  $original_receipt = $pdf_variables["original_receipt"];
  $pdf_receipt_files_path = $pdf_variables["pdf_receipt_files_path"];
  $line_1 = $pdf_variables["line_1"];
  $source_funds = $pdf_variables["source_funds"];
  $amount = $pdf_variables["amount"];
  $display_date = $pdf_variables["display_date"];
  $receipt_number = $pdf_variables["receipt_number"];
  $displayname = $pdf_variables["displayname"];
  $address_line_1 = $pdf_variables["address_line_1"];
  $address_line_2 = $pdf_variables["address_line_2"];
  $address_line_3 = $pdf_variables["address_line_3"];

  // Middle center section
  if ( !$original_receipt ) $pdf->Image($pdf_receipt_files_path . 'duplicate_copy.png', $mymargin_left + 65, $mymargin_top, '', 45);

  // Top left section
  $pdf->Image(variable_get('cdntaxreceipts_logo', $pdf_receipt_files_path . 'your-logo.png'), $mymargin_left, $mymargin_top, '', 30);

  // Top right section
  $pdf->SetFont('Helvetica', '', 8);
  $pdf->SetY($mymargin_top);
  $pdf->Write(10, variable_get('cdntaxreceipts_org_name', ''), '', 0, 'R', TRUE, 0, FALSE, FALSE, 0);
  $pdf->SetY($mymargin_top + 4);
  $pdf->Write(10, variable_get('cdntaxreceipts_address_line1', ''), '', 0, 'R', TRUE, 0, FALSE, FALSE, 0);
  $pdf->SetY($mymargin_top + 8);
  $pdf->Write(10, variable_get('cdntaxreceipts_address_line2', ''), '', 0, 'R', TRUE, 0, FALSE, FALSE, 0);
  $pdf->SetY($mymargin_top + 12);
  $pdf->Write(10, 'Tel: ' . variable_get('cdntaxreceipts_tel', '') . '; Fax: ' . variable_get('cdntaxreceipts_fax', ''), '', 0, 'R', TRUE, 0, FALSE, FALSE, 0);
  $pdf->SetFont('Helvetica', 'I', 8);
  $pdf->SetY($mymargin_top + 16);
  $pdf->Write(10, 'Email: ' . variable_get('cdntaxreceipts_email', '') . '; Website: ' . variable_get('cdntaxreceipts_web', ''), '', 0, 'R', TRUE, 0, FALSE, FALSE, 0);

  // Right section
  $x_detailscolumn = 120;
  $y_detailscolumnstart = 24;
  $background_image = variable_get('cdntaxreceipts_bg', '');
  if ( $background_image ) $pdf->Image(variable_get('cdntaxreceipts_bg', ''), $mymargin_left + $x_detailscolumn, $mymargin_top + $y_detailscolumnstart, '', 50);
  $pdf->SetFont('Helvetica', 'B', 8);
  $pdf->SetXY($mymargin_left + $x_detailscolumn, $mymargin_top + $y_detailscolumnstart + 4);
  $pdf->Write(10, "Receipt No: " . $receipt_number);
  $pdf->SetXY($mymargin_left + $x_detailscolumn, $mymargin_top + $y_detailscolumnstart + 10);
  $today = date("Y-m-d");
  $pdf->Write(10, "Issue Date: " . $today);
  $pdf->SetXY($mymargin_left + $x_detailscolumn, $mymargin_top + $y_detailscolumnstart + 16);
  $pdf->Write(10, 'Charitable Registration ' . variable_get('cdntaxreceipts_charitable_no', ''));
  $pdf->SetXY($mymargin_left + $x_detailscolumn, $mymargin_top + $y_detailscolumnstart + 22);
  $pdf->Write(10, "Received on: " . $display_date);
  $pdf->SetXY($mymargin_left + $x_detailscolumn, $mymargin_top + $y_detailscolumnstart + 28);
  $pdf->Write(10, "Eligible Gift Amount:  $" . number_format($amount, 2));
  $pdf->SetXY($mymargin_left + $x_detailscolumn, $mymargin_top + $y_detailscolumnstart + 34);

  if (strlen($source_funds) < 36) {
    $pdf->Write(10, "Source: " . $source_funds);
  }
  else {
    $source_funds_words = explode(" ", substr($source_funds, 0, 36));
    $source_funds_lastbit = array_pop($source_funds_words);
    $pdf->Write(10, "Source: " . implode(" ", $source_funds_words));
    $source_funds_count = count($source_funds_words);
    $source_funds_nextline = array_splice(explode(" ", $source_funds), $source_funds_count);
    $pdf->SetXY($mymargin_left + $x_detailscolumn + 16, $mymargin_top + $y_detailscolumnstart + 38);
    $pdf->Write(10, implode(" ", $source_funds_nextline));
  }

  // Left section
  $pdf->SetFont('Helvetica', 'B', 8);
  $pdf->SetY($mymargin_top + $y_detailscolumnstart + 10);
  $pdf->Write(10, "Received from: ", '', 0, 'L', TRUE, 0, FALSE, FALSE, 0);
  $pdf->SetFont('Helvetica', 'B', 10);
  $pdf->SetY($mymargin_top + $y_detailscolumnstart + 22);
  $pdf->Write(10, strtoupper($displayname), '', 0, 'L', TRUE, 0, FALSE, FALSE, 0);
  $pdf->SetFont('Helvetica', '', 10);
  $pdf->SetY($mymargin_top + $y_detailscolumnstart + 26);
  $pdf->Write(10, strtoupper($address_line_1), '', 0, 'L', TRUE, 0, FALSE, FALSE, 0);
  $pdf->SetY($mymargin_top + $y_detailscolumnstart + 30);
  $pdf->Write(10, strtoupper($address_line_2), '', 0, 'L', TRUE, 0, FALSE, FALSE, 0);
  $pdf->SetY($mymargin_top + $y_detailscolumnstart + 34);
  $pdf->Write(10, strtoupper($address_line_3), '', 0, 'L', TRUE, 0, FALSE, FALSE, 0);

  // Bottom left section
  $pdf->SetFont('Helvetica', 'B', 8);
  $pdf->SetY($mymargin_top + 78);
  $pdf->Write(10, $line_1, '', 0, 'L', TRUE, 0, FALSE, FALSE, 0);
  $pdf->SetFont('Helvetica', '', 8);
  $pdf->SetY($mymargin_top + 82, 0, 'L', TRUE, 0, FALSE, FALSE, 0);
  $pdf->Write(10, 'Canadian Revenue Agency: www.cra-arc.gc.ca/charities');

  // Bottom center section
  $pdf->SetFont('Helvetica', 'B', 8);
  $pdf->SetXY($mymargin_left + 92, $mymargin_top + 82);
  $pdf->Write(10, 'Thank you!');

  // Bottom right section
  $pdf->Image(variable_get('cdntaxreceipts_signature', $pdf_receipt_files_path . 'authorized-signature.png'), $mymargin_left + 137, $mymargin_top + 76, '', 15);
  $pdf->Line($mymargin_left + 136, $mymargin_top + 87, $mymargin_left + 186, $mymargin_top + 87);
  $pdf->SetXY($mymargin_left + 148, $mymargin_top + 84);
  $pdf->SetFont("Helvetica", "I", 7);
  $pdf->Write(10, 'Authorized Signature');

  // Line at the bottom
  $pdf->Line($mymargin_left, $mymargin_top + 93, 198, $mymargin_top + 93, 'dash');
}

function civicrm_cdntaxreceipts_issueTaxReceipt( $contribution, $original_receipt ) {

  // grab the various objects that are relevant to the mail message...
  require_once 'CRM/Contribute/DAO/ContributionType.php';
  $contributiontype =  new CRM_Contribute_DAO_ContributionType();
  $contributiontype->id = $contribution->contribution_type_id;
  if (! $contributiontype->find(TRUE)) {
    CRM_Core_Error::fatal( "Could not find corresponding contribution type." );
    return;
  }

  require_once 'CRM/Contact/BAO/Contact.php';
  list( $displayname, $email ) = CRM_Contact_BAO_Contact::getContactDetails( $contribution->contact_id );

  // generate the receipt

  list($pdf_file, $user_friendly) = civicrm_cdntaxreceipts_generateTaxReceipt( $contribution, $original_receipt );
  $attachment = array(
    'fullPath' => $pdf_file,
    'mime_type' => 'application/pdf',
    'cleanName' => $user_friendly,
  );

  // form a mailParams array to pass to the CiviCRM mail utility
  $email_message = variable_get('cdntaxreceipts_message', '');
  $site_email = variable_get('site_mail', '');
  $site_name = variable_get('site_name', '');

  $email_subject_admin = variable_get('cdntaxreceipts_subject', '');
  $padded_number = str_pad($contribution->id, 8, 0, STR_PAD_LEFT);
  $receipt_number = variable_get('cdntaxreceipts_prefix', '') . $padded_number;
  $email_subject = $email_subject_admin . " " . $receipt_number;

  $email_from = variable_get('cdntaxreceipts_from', '');
  $email_archive = variable_get('cdntaxreceipts_archive', '');

  $mailparams = array(
    'from' => $site_name . ' <' . $email_from . '> ',
    'toName' => $displayname,
    'toEmail' => $email_archive,
    'cc' => $email,
    'attachments' => array( $attachment ),
    'subject' => $email_subject,
    'text' => $email_message,
    'returnPath' => $email_from,
  );

  $ret = CRM_Utils_Mail::send( $mailparams );

  civicrm_cdntaxreceipts_cleanup( $pdf_file );
}

function civicrm_cdntaxreceipts_cleanup($pdf_file) {
  unlink($pdf_file);
}

function civicrm_cdntaxreceipts_taxReceiptSent( &$contribution ) {

  $query = "
    SELECT contribution_id
    FROM civicrm_cdntaxreceipts
    WHERE contribution_id = %1 AND received_date IS NOT NULL";

  $dao =& CRM_Core_DAO::executeQuery( $query, array( 1 => array( $contribution->id, "Integer" ) ) );
  return $dao->fetch();

}

function civicrm_cdntaxreceipts_recordTaxReceipt( &$contribution ) {

  $query = "
    INSERT INTO civicrm_cdntaxreceipts (contribution_id, received_date)
    VALUES (%1, %2)";

  $params = array( 1 => array( $contribution->id, "Integer" ),
                   2 => array( CRM_Utils_Date::isoToMysql( $contribution->receive_date ), "Timestamp" ) );
  $dao =& CRM_Core_DAO::executeQuery( $query, $params );

}


