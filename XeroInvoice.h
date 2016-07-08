#ifndef XEROINVOICE_H
#define XEROINVOICE_H

#include <QObject>
#include <QDateTime>
#include <QJsonObject>

class XeroInvoice : public QObject
{
   Q_OBJECT

   Q_PROPERTY(QString id READ id CONSTANT)
   Q_PROPERTY(InvoiceType type READ type CONSTANT)
   Q_PROPERTY(InvoiceStatus status READ status CONSTANT)
   Q_PROPERTY(QString number READ number CONSTANT)
   Q_PROPERTY(QString reference READ reference CONSTANT)
   Q_PROPERTY(QDateTime date READ date CONSTANT)
   Q_PROPERTY(QDateTime dueDate READ dueDate CONSTANT)
   Q_PROPERTY(QString amountDue READ amountDue CONSTANT)
   Q_PROPERTY(QString amountPaid READ amountPaid CONSTANT)
   Q_PROPERTY(QString amountCredited READ amountCredited CONSTANT)
   Q_PROPERTY(QString url READ url CONSTANT)

public:

   enum InvoiceType
   {
      INVOICE_TYPE_UNKNOWN,
      INVOICE_TYPE_ACCOUNTS_PAYABLE, // ACCPAY	A bill – commonly known as a Accounts Payable or supplier invoice
      INVOICE_TYPE_ACCOUNTS_RECEIVABLE, // ACCREC	A sales invoice – commonly known as an Accounts Receivable or customer invoice
   };
   Q_ENUMS( InvoiceType )

   enum InvoiceStatus
   {
      INVOICE_STATUS_UNKNOWN,
      INVOICE_STATUS_DRAFT, // A Draft Invoice (default)
      INVOICE_STATUS_SUBMITTED, // An Awaiting Approval Invoice
      INVOICE_STATUS_DELETED, // A Deleted Invoice
      INVOICE_STATUS_AUTHORISED, // An Invoice that is Approved and Awaiting Payment OR partially paid
      INVOICE_STATUS_PAID, // An Invoice that is completely Paid
      INVOICE_STATUS_VOIDED, // A Voided Invoice
   };
   Q_ENUMS( InvoiceStatus )

   XeroInvoice(const QJsonObject& json, QObject *parent = 0);

   QString id() const;
   InvoiceType type() const;
   InvoiceStatus status() const;
   QString number() const;
   QString reference() const;
   QDateTime date() const;
   QDateTime dueDate() const;
   QString amountDue() const;
   QString amountPaid() const;
   QString amountCredited() const;
   QString url() const;

private:
   QJsonObject json_;
};

#endif // XEROINVOICE_H
