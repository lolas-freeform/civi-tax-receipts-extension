This module creates Canadian (CDN) Tax Receipts for charitable donations.
Receipts (for both offline and online contributions) are MANUALLY issued via a "Issue Tax Receipt" button in the View Contribution screen.

It was developed in close cooperation with a senior accountant (partner at a major firm) who consulted CRA on a number of occasions - though we
are confident that the module is meeting CRA's current guidelines for Tax Receipts re: Charitable Donations, all users of this module should remain
dilligent about keeping informed on CRA's guidelines as they may change in future.

For more on what and how, please see:
http://wiki.civicrm.org/confluence/display/CRM/Canadian+Charitable+Receipts

Drupal Installation:
---------------------
1.  Unzip the module in your Drupal /modules
2.  Enable the module in Drupal
3.  Go to Admin > Site Configuration > CiviCRM CDN Tax Receipts
    a. enter your Organization's details: including name/contact and address info and charitiable
       registration number
    b. specify items for Receipt configuration: including Receipt Prefix, upload your organization's logo (.png or .jpg),
       upload a scanned signature (.png or .jpg) of someone authorized to sign tax receipts, upload a watermark
       (background image) (.png or .jpg)
    c. enter the details for the Email message to accompany the tax receipt: including, subject and message, Email From: and also
       an Archive Email address

TCPDF Installation:
---------------------
For the 7.x branch, I've replaced UFPDF with TCPDF.

Instructions:
1.  Download TCPDF from: https://sourceforge.net/projects/tcpdf/files/ and unzip it
2.  Drop tcpdf/ in: /modules/civicrm_cdntaxreceipts/
3.  AND rename it to: this_tcpdf/

CiviCRM Setup:
--------------------
To configure tax receipts in CiviCRM:
1.  Go to CiviCRM > Administer > CiviContribute > Contribution Types
2.  Make sure any tax-eligible donation types are marked "Deductible"
    (no PDF tax receipt will be sent for non-deductible contributions)

Use:
--------------------
This module adds a "Issue Tax Receipt" button to the View Contribution screen. When clicked a set of Tax Receipts are issued:
1.  A copy is downloaded to the (staff) computer it is issued from (and can be printed to be mailed)
2.  A copy is Emailed to the Email archive address
3.  A copy is Emailed to the donor (only succeeds if donor Email is in the Contact record)

If the "Issue Tax Receipt" button is clicked again a second set of receipts - now marked "Duplicate Copy" will be issued. The Source
field is printed on the Tax receipt and can be used to eg add a note if this receipt replaces one that was issued previously.
