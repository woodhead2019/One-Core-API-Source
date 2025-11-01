@ stub SLCallServer
@ stub SLpAuthenticateGenuineTicketResponse
@ stub SLpBeginGenuineTicketTransaction
@ stub SLpClearActivationInProgress
@ stub SLpDepositDownlevelGenuineTicket
@ stub SLpDepositTokenActivationResponse
@ stub SLpGenerateTokenActivationChallenge
@ stub SLpGetGenuineBlob
@ stub SLpGetGenuineLocal
@ stub SLpGetLicenseAcquisitionInfo
@ stub SLpGetMSPidInformation
@ stub SLpGetMachineUGUID
@ stub SLpGetTokenActivationGrantInfo
@ stub SLpIAActivateProduct
@ stub SLpIsCurrentInstalledProductKeyDefaultKey
@ stub SLpProcessVMPipeMessage
@ stub SLpSetActivationInProgress
@ stub SLpTriggerServiceWorker
@ stub SLpVLActivateProduct
@ stdcall SLClose(ptr)
@ stdcall SLConsumeRight(ptr ptr ptr wstr ptr)
@ stub SLDepositMigrationBlob
@ stub SLDepositOfflineConfirmationId
@ stub SLDepositOfflineConfirmationIdEx
@ stub SLDepositStoreToken
@ stub SLFireEvent
@ stdcall SLGatherMigrationBlob(long wstr ptr)
@ stub SLGatherMigrationBlobEx
@ stub SLGenerateOfflineInstallationId
@ stub SLGenerateOfflineInstallationIdEx
@ stub SLGetActiveLicenseInfo
@ stub SLGetApplicationInformation
@ stub SLGetApplicationPolicy
@ stub SLGetAuthenticationResult
@ stub SLGetEncryptedPIDEx
@ stub SLGetGenuineInformation
@ stdcall SLGetInstalledProductKeyIds(ptr ptr ptr ptr)
@ stub SLGetLicense
@ stub SLGetLicenseFileId
@ stub SLGetLicenseInformation
@ stdcall SLGetLicensingStatusInformation(ptr ptr ptr wstr ptr ptr)
@ stub SLGetPKeyId
@ stub SLGetPKeyInformation
@ stdcall SLGetPolicyInformation(ptr wstr ptr ptr ptr)
@ stdcall SLGetPolicyInformationDWORD(ptr wstr ptr)
@ stdcall SLGetProductSkuInformation(ptr ptr wstr ptr ptr ptr)
@ stdcall SLGetSLIDList(ptr long ptr long ptr ptr)
@ stub SLGetServiceInformation
@ stdcall SLInstallLicense(ptr long ptr ptr)
@ stdcall SLInstallProofOfPurchase(ptr wstr wstr long ptr ptr)
@ stub SLInstallProofOfPurchaseEx
@ stub SLIsGenuineLocalEx
@ stdcall SLLoadApplicationPolicies(ptr ptr long ptr)
@ stdcall SLOpen(ptr)
@ stdcall SLPersistApplicationPolicies(ptr ptr long)
@ stub SLPersistRTSPayloadOverride
@ stub SLReArm
@ stub SLRegisterEvent
@ stub SLRegisterPlugin
@ stdcall SLSetAuthenticationData(ptr ptr ptr)
@ stub SLSetCurrentProductKey
@ stub SLSetGenuineInformation
@ stdcall SLUninstallLicense(ptr ptr)
@ stdcall SLUninstallProofOfPurchase(ptr ptr)
@ stdcall SLUnloadApplicationPolicies(ptr long)
@ stub SLUnregisterEvent
@ stub SLUnregisterPlugin
