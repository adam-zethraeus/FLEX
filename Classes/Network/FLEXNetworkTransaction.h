//
//  FLEXNetworkTransaction.h
//  Flipboard
//
//  Created by Ryan Olson on 2/8/15.
//  Copyright (c) 2020 FLEX Team. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Firestore.h"

typedef NS_ENUM(NSInteger, FLEXNetworkTransactionState) {
    FLEXNetworkTransactionStateUnstarted = -1,
    /// This is the default; it's usually nonsense for a request to be marked as "unstarted"
    FLEXNetworkTransactionStateAwaitingResponse = 0,
    FLEXNetworkTransactionStateReceivingData,
    FLEXNetworkTransactionStateFinished,
    FLEXNetworkTransactionStateFailed
};

typedef NS_ENUM(NSUInteger, FLEXWebsocketMessageDirection) {
    FLEXWebsocketIncoming = 1,
    FLEXWebsocketOutgoing,
};

/// The shared base class for all types of network transactions.
/// Subclasses should implement the descriptions and details properties, and assign a thumbnail.
@interface FLEXNetworkTransaction : NSObject

+ (instancetype)withStartTime:(NSDate *)startTime;

+ (NSString *)readableStringFromTransactionState:(FLEXNetworkTransactionState)state;

@property (nonatomic) NSError *error;
/// Subclasses can override to provide error state based on response data as well
@property (nonatomic, readonly) BOOL displayAsError;
@property (nonatomic, readonly) NSDate *startTime;

@property (nonatomic) FLEXNetworkTransactionState state;
@property (nonatomic) int64_t receivedDataLength;
/// A small thumbnail to preview the type of/the response
@property (nonatomic) UIImage *thumbnail;

/// The most prominent line of the cell. Typically a URL endpoint or other distinguishing attribute.
/// This line turns red when the transaction indicates an error.
@property (nonatomic, readonly) NSString *primaryDescription;
/// Something less important, such as a blob of data or the URL's domain.
@property (nonatomic, readonly) NSString *secondaryDescription;
/// Minor details to display at the bottom of the cell, such as a timestamp, HTTP method, or status.
@property (nonatomic, readonly) NSString *tertiaryDescription;

/// The string to copy when the user selects the "copy" action
@property (nonatomic, readonly) NSString *copyString;

/// Whether or not this request should show up when the user searches for a given string
- (BOOL)matchesQuery:(NSString *)filterString;

@end

/// The shared base class for all NSURL-API-related transactions.
/// Descriptions are generated by this class using the URL provided by subclasses.
@interface FLEXURLTransaction : FLEXNetworkTransaction

+ (instancetype)withRequest:(NSURLRequest *)request startTime:(NSDate *)startTime;

@property (nonatomic, readonly) NSURLRequest *request;
/// Subclasses should implement for when the transaction is complete
@property (nonatomic, readonly) NSArray<NSString *> *details;

@end


@interface FLEXHTTPTransaction : FLEXURLTransaction

+ (instancetype)request:(NSURLRequest *)request identifier:(NSString *)requestID;

@property (nonatomic, readonly) NSString *requestID;
@property (nonatomic) NSURLResponse *response;
@property (nonatomic, copy) NSString *requestMechanism;

@property (nonatomic) NSTimeInterval latency;
@property (nonatomic) NSTimeInterval duration;

/// Populated lazily. Handles both normal HTTPBody data and HTTPBodyStreams.
@property (nonatomic, readonly) NSData *cachedRequestBody;

@end


@interface FLEXWebsocketTransaction : FLEXURLTransaction

+ (instancetype)withMessage:(NSURLSessionWebSocketMessage *)message
                       task:(NSURLSessionWebSocketTask *)task
                  direction:(FLEXWebsocketMessageDirection)direction API_AVAILABLE(ios(13.0));

+ (instancetype)withMessage:(NSURLSessionWebSocketMessage *)message
                       task:(NSURLSessionWebSocketTask *)task
                  direction:(FLEXWebsocketMessageDirection)direction
                  startTime:(NSDate *)started API_AVAILABLE(ios(13.0));

//@property (nonatomic, readonly) NSURLSessionWebSocketTask *task;
@property (nonatomic, readonly) NSURLSessionWebSocketMessage *message API_AVAILABLE(ios(13.0));
@property (nonatomic, readonly) FLEXWebsocketMessageDirection direction API_AVAILABLE(ios(13.0));

@property (nonatomic, readonly) int64_t dataLength API_AVAILABLE(ios(13.0));

@end


typedef NS_ENUM(NSUInteger, FLEXFIRTransactionDirection) {
    FLEXFIRTransactionDirectionNone,
    FLEXFIRTransactionDirectionPush,
    FLEXFIRTransactionDirectionPull,
};

typedef NS_ENUM(NSUInteger, FLEXFIRRequestType) {
    FLEXFIRRequestTypeNotFirebase,
    FLEXFIRRequestTypeFetchQuery,
    FLEXFIRRequestTypeFetchDocument,
    FLEXFIRRequestTypeSetData,
    FLEXFIRRequestTypeUpdateData,
    FLEXFIRRequestTypeDeleteDocument,
};

NSString * FLEXStringFromFIRRequestType(FLEXFIRRequestType type);

@interface FLEXFirebaseSetDataInfo : NSObject
/// The data that was set
@property (nonatomic, readonly) NSDictionary *documentData;
/// \c nil if \c mergeFields is populated
@property (nonatomic, readonly) NSNumber *merge;
/// \c nil if \c merge is populated
@property (nonatomic, readonly) NSArray *mergeFields;
@end

@interface FLEXFirebaseTransaction : FLEXNetworkTransaction

+ (instancetype)queryFetch:(FIRQuery *)initiator;
+ (instancetype)documentFetch:(FIRDocumentReference *)initiator;
+ (instancetype)setData:(FIRDocumentReference *)initiator
                   data:(NSDictionary *)data
                  merge:(NSNumber *)merge
            mergeFields:(NSArray *)mergeFields;
+ (instancetype)updateData:(FIRDocumentReference *)initiator data:(NSDictionary *)data;
+ (instancetype)deleteDocument:(FIRDocumentReference *)initiator;

@property (nonatomic, readonly) FLEXFIRTransactionDirection direction;
@property (nonatomic, readonly) FLEXFIRRequestType requestType;

@property (nonatomic, readonly) id initiator;
@property (nonatomic, readonly) FIRQuery *initiator_query;
@property (nonatomic, readonly) FIRDocumentReference *initiator_doc;
@property (nonatomic, readonly) FIRCollectionReference *initiator_collection;

/// Only used for fetch types
@property (nonatomic, copy) NSArray<FIRDocumentSnapshot *> *documents;
/// Only used for the "set data" type
@property (nonatomic, readonly) FLEXFirebaseSetDataInfo *setDataInfo;
/// Only used for the "update data" type
@property (nonatomic, readonly) NSDictionary *updateData;

@property (nonatomic, readonly) NSString *path;

//@property (nonatomic, readonly) NSString *responseString;
//@property (nonatomic, readonly) NSDictionary *responseObject;

@end
